// Fill out your copyright notice in the Description page of Project Settings.


#include "PunchCameraShake.h"

UPunchCameraShake::UPunchCameraShake()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	RotOscillation.Pitch.Amplitude = FMath::RandRange(0.2f, 0.4f);
	RotOscillation.Pitch.Frequency = FMath::RandRange(25.0f, 35.0f);

	RotOscillation.Yaw.Amplitude = FMath::RandRange(0.2f, 0.4f);
	RotOscillation.Yaw.Frequency = FMath::RandRange(25.0f, 35.0f);
}