/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Statements", "[Statements]")
{
	SECTION("Test comments")
	{
		const char * scriptText =
			u8R"(

			-- Single line comment

			--- Multiline 
				comment ---
				
			---------
			Alternate
			multiline
			comment
			---------

---------
No indentation
multiline comment
---------

			set a --- some comment --- to 123
   
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 123);
	}

	SECTION("Test variables and basic statements")
	{
		const char * scriptText =
			u8R"(

			set a to "Hello world!"
			set b to 5.5
			set c to 123
			set d to true
			set e to null

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "Hello world!");
		REQUIRE(script->GetVariable("b") == 5.5);
		REQUIRE(script->GetVariable("c") == 123);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == nullptr);
	}

	SECTION("Test variables case insensitivity")
	{
		const char * scriptText =
			u8R"(

			set a b c to "Hello world!"
			set x y z to A B C

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a b c") == "Hello world!");
		REQUIRE(script->GetVariable("x y z") == "Hello world!");
		REQUIRE(script->GetVariable("A B C") == "Hello world!");
		REQUIRE(script->GetVariable("X Y Z") == "Hello world!");
	}

	SECTION("Test keyword case insensitivity")
	{
		const char * scriptText =
			u8R"(

			Set a TO True
			If a = TRUE
				SET a To 12345
			END

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 12345);
	}

	SECTION("Test multiple word variables and basic statements")
	{
		const char * scriptText =
			u8R"(

			set a a to "Hello world!"
			set b b to 5.5
			set c c to 123
			set d d to true
			set e e to null

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a a") == "Hello world!");
		REQUIRE(script->GetVariable("b b") == 5.5);
		REQUIRE(script->GetVariable("c c") == 123);
		REQUIRE(script->GetVariable("d d") == true);
		REQUIRE(script->GetVariable("e e") == nullptr);
	}

	SECTION("Test multiple word variables with keywords")
	{
		const char * scriptText =
			u8R"(

			set some string to "Hello world!"
			set some number to 5.5
			set some integer to 123
			set some boolean to true
			set some null to null

			external some other string

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("some string") == "Hello world!");
		REQUIRE(script->GetVariable("some number") == 5.5);
		REQUIRE(script->GetVariable("some integer") == 123);
		REQUIRE(script->GetVariable("some boolean") == true);
		REQUIRE(script->GetVariable("some null") == nullptr);
	}

	SECTION("Test variable and property type")
	{
		const char * scriptText =
			u8R"(

			set a to 123
			set private b to 234
			set c to a type
			set d to false
			if a type = b type
				set d to true
			end
			set e to false
			if a type = integer
				set e to true
			end
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").IsValType());
		REQUIRE(script->GetVariable("c").GetValType() == ValueType::Integer);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
	}

	SECTION("Test alternate numeric form parsing")
	{
		const char * scriptText =
			u8R"(
	
			set a to -375.5
			set b to .11111
			set c to -999
			set d to 00001
			set e to +123

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == -375.5);
		REQUIRE(script->GetVariable("b").GetNumber() == Approx(.11111));
		REQUIRE(script->GetVariable("c") == -999);
		REQUIRE(script->GetVariable("d") == 1);
		REQUIRE(script->GetVariable("e") == 123);
	}

	SECTION("Test variable scope")
	{
		const char * scriptText =
			u8R"(

			-- Scope test - a will not be visible after end
			begin
				set a to 42
			end

			-- Scope test - g will be visible inside scope block
			set b to 999
			begin
				set b to 55
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == nullptr);
		REQUIRE(script->GetVariable("b") == 55);
	}

	SECTION("Test increment and decrement operators")
	{
		const char * scriptText =
			u8R"(
	
			-- Increment and decrement test
			set a to 1
			increment a
			set b to 1
			decrement b
			set c to 1
			increment c by 4
			set d to 1
			decrement d by 3
			set e to 1
			increment e by 4 * (2 + 2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 2);
		REQUIRE(script->GetVariable("b") == 0);
		REQUIRE(script->GetVariable("c") == 5);
		REQUIRE(script->GetVariable("d") == -2);
		REQUIRE(script->GetVariable("e") == 17);
	}


	SECTION("Test wait statement")
	{
		const char * scriptText =
			u8R"(
	
			set private counter to 0
			
			function counter to finished
				increment counter
				return counter >= 10
			end

			loop while not counter to finished
				wait
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("counter") == 10);
	}

	SECTION("Test wait while statement")
	{
		const char * scriptText =
			u8R"(
	
			set private counter to 0
			
			function counter to finished
				increment counter
				return counter >= 10
			end

			wait while not counter to finished

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("counter") == 10);
	}

	SECTION("Test wait until statement")
	{
		const char * scriptText =
			u8R"(
	
			set private counter to 0
			
			function counter to finished
				increment counter
				return counter >= 10
			end

			wait until counter to finished

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("counter") == 10);
	}

	SECTION("Test external statement")
	{
		const char * scriptText =
			u8R"(
	
			external some var
			set another var to some var

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		script->SetVariable("some var", 123);
		script->Execute();
		REQUIRE(script->GetVariable("another var") == 123);
	}

	SECTION("Test return in root scope")
	{
		const char * scriptText =
			u8R"(

			-- Does nothing, but is legal
			return

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
	}

	SECTION("Test return in root scope with non-executing code")
	{
		const char * scriptText =
			u8R"(
	
			return

			-- This code will never be reached
			set x to 123

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("x") == nullptr);
	}

	SECTION("Test return in if statement")
	{
		const char * scriptText =
			u8R"(

			if (true)
				return
			end

			-- This code will never be reached
			set x to 123

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("x") == nullptr);
	}

	SECTION("Test return in loop statement")
	{
		const char * scriptText =
			u8R"(

			set x to 0
			loop i from 1 to 1000000
				if (i = 100)
					return
				end
				increment x
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("x") < 100);
	}

	SECTION("Test apostrophe #1")
	{
		const char * scriptText =
			u8R"(

			import core

			set a to "Hello"
			set b to a's size

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b") == 5);
	}

	SECTION("Test apostrophe #2")
	{
		const char * scriptText =
			u8R"(

			import core

			set guess to "Hello"
			set b to guess' size

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b") == 5);
	}

	SECTION("Test apostrophe #3")
	{
		const char * scriptText =
			u8R"(

			import core

			set 'some val' to "Hello"
			set b to 'some val's size

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b") == 5);
	}

}
