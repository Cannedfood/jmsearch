workspace "JMsearch"

language "C++" flags "C++14"

configurations {
	"debug",
	"release"
}

filter "configurations:debug"
	optimize "Debug"
	symbols "On"
filter "configurations:release"
	optimize "Speed"
filter "*"

project "jmsearch"
	kind "ConsoleApp"
	files "src/**"
