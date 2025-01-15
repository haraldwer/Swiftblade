#pragma once
#include "DBData.h"

namespace DB
{
	enum class AuthMethod : uint8
	{
		DEVICE,
		// EMAIL,
		// STEAM,
		// GOOGLE,
	};

	struct AuthData : DB::DBData<AuthData>  
	{
		PROPERTY_D(uint8, Method, static_cast<uint8>(AuthMethod::DEVICE));
		PROPERTY(String, User);
		PROPERTY(String, Token);
		bool Create = false;
	};

	struct OnLoginSuccess
	{
		String SessionID;
	};

	struct OnLoginError : OnError {};
}

