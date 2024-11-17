#pragma once
#include "DBData.h"

enum class AuthMethod : uint8
{
	DEVICE,
	// EMAIL,
	// STEAM,
	// GOOGLE,
};

struct AuthData : DB::DBData<AuthData>  
{
	//PROPERTY_D(AuthMethod, Method, AuthMethod::DEVICE);
	PROPERTY(String, User);
	PROPERTY(String, Token);
};
