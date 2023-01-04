// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

JPH_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// ProfileThread
//////////////////////////////////////////////////////////////////////////////////////////

ProfileThread::ProfileThread(const string_view &inThreadName) :
	mThreadName(inThreadName)
{
	Profiler::sInstance->AddThread(this);
}

ProfileThread::~ProfileThread()
{
	Profiler::sInstance->RemoveThread(this);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ProfileMeasurement
//////////////////////////////////////////////////////////////////////////////////////////

ProfileMeasurement::ProfileMeasurement(const char *inName, uint32 inColor)	
{
	if (ProfileThread::sInstance == nullptr)
	{
		// Thread not instrumented
		mSample = nullptr;
	}
	else if (ProfileThread::sInstance->mCurrentSample < ProfileThread::cMaxSamples)
	{
		// Get pointer to write data to
		mSample = &ProfileThread::sInstance->mSamples[ProfileThread::sInstance->mCurrentSample++];

		// Start constructing sample (will end up on stack)
		mTemp.mName = inName;
		mTemp.mColor = inColor;

		// Collect start sample last
		mTemp.mStartCycle = GetProcessorTickCount();
	}
	else
	{
		// Out of samples
		if (!sOutOfSamplesReported)
		{
			Trace("ProfileMeasurement: Too many samples, some data will be lost!");
			sOutOfSamplesReported = true;
		}
		mSample = nullptr;
	}
}

ProfileMeasurement::~ProfileMeasurement()
{
	if (mSample != nullptr)
	{
		// Finalize sample
		mTemp.mEndCycle = GetProcessorTickCount();

		// Write it to the memory buffer bypassing the cache
		static_assert(sizeof(ProfileSample) == 32, "Assume 32 bytes");
		static_assert(alignof(ProfileSample) == 16, "Assume 16 byte alignment");
	#if defined(JPH_USE_SSE)
		const __m128i *src = reinterpret_cast<const __m128i *>(&mTemp);
		__m128i *dst = reinterpret_cast<__m128i *>(mSample);
		__m128i val = _mm_loadu_si128(src);
		_mm_stream_si128(dst, val);
		val = _mm_loadu_si128(src + 1);
		_mm_stream_si128(dst + 1, val);
	#elif defined(JPH_USE_NEON)
		const int *src = reinterpret_cast<const int *>(&mTemp);
		int *dst = reinterpret_cast<int *>(mSample);
		int32x4_t val = vld1q_s32(src);
		vst1q_s32(dst, val);
		val = vld1q_s32(src + 4);
		vst1q_s32(dst + 4, val);
	#else
		memcpy(mSample, &mTemp, sizeof(ProfileSample));
	#endif
		mSample = nullptr;
	}
}

JPH_NAMESPACE_END
