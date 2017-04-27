workspace "JMsearch"

language "C++" flags "C++14"

configurations {
	"debug",
	"release"
}

filter "configurations:debug"
	optimize "Debug"
	symbols "On"
	defines "DEBUG=1" -- Only changes the output
filter "configurations:release"
	optimize "Speed"
filter "*"

project "jmsearch"
	kind "ConsoleApp"
	files "src/**"
	links "dl"
