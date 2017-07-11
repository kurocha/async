
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "1.3"

# Project Metadata

define_project "async" do |project|
	project.title = "Async"
	project.summary = 'Implements the reactor pattern using fibers.'
	
	project.license = "MIT License"
	
	project.add_author 'Samuel Williams', email: 'samuel.williams@oriontransfer.co.nz'
	
	project.version = "1.0.0"
end

define_target 'async-library' do |target|
	target.build do
		source_root = target.package.path + 'source'
		
		copy headers: source_root.glob('Async/**/*.hpp')
		
		build static_library: "Async", source_files: source_root.glob('Async/**/*.cpp')
	end
	
	target.depends 'Build/Files'
	target.depends 'Build/Clang'
	
	target.depends :platform
	target.depends "Language/C++11", private: true
	
	target.depends "Library/Memory"
	target.depends "Library/Time"
	target.depends "Library/Concurrent"
	
	target.depends "Build/Files"
	target.depends "Build/Clang"
	
	target.provides "Library/Async" do
		append linkflags [
			->{install_prefix + 'lib/libAsync.a'},
		]
	end
end

define_target "async-tests" do |target|
	target.build do |*arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'Async', source_files: test_root.glob('Async/**/*.cpp'), arguments: arguments
	end
	
	target.depends "Language/C++11", private: true
	
	target.depends "Library/UnitTest"
	target.depends "Library/Async"
	
	target.provides "Test/Async"
end

# Configurations

define_configuration "async" do |configuration|
	configuration[:source] = "http://github.com/kurocha/"
		
	# Provides all the build related infrastructure:
	configuration.require "platforms"
	configuration.require "build-files"
	
	configuration.require "concurrent"
	configuration.require "time"
	configuration.require "memory"
	
	# Provides unit testing infrastructure and generators:
	configuration.require "unit-test"
	
	# Provides some useful C++ generators:
	configuration.require "generate-travis"
	configuration.require "generate-project"
	configuration.require "generate-cpp-class"
end
