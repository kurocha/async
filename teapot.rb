
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "3.0"

# Project Metadata

define_project "async" do |project|
	project.title = "Async"
	project.summary = 'Implements the reactor pattern using fibers.'
	
	project.license = "MIT License"
	
	project.add_author 'Samuel Williams', email: 'samuel.williams@oriontransfer.co.nz'
	
	project.version = "1.0.0"
end

# Build Targets

define_target 'async-library' do |target|
	target.depends "Language/C++14"
	
	target.depends "Library/Memory"
	target.depends "Library/Time", public: true
	target.depends "Library/Concurrent", public: true
	
	target.provides "Library/Async" do
		source_root = target.package.path + 'source'
		
		library_path = build static_library: "Async", source_files: source_root.glob('Async/**/*.cpp')
		
		append linkflags library_path
		append header_search_paths source_root
	end
end

define_target "async-tests" do |target|
	target.depends 'Library/UnitTest'
	target.depends "Language/C++14", private: true
	
	target.depends "Library/Async"
	target.depends "Library/Parallel"
	
	target.provides "Test/Async" do |*arguments|
		test_root = target.package.path + 'test'
		
		run source_files: test_root.glob('Async/**/*.cpp'), arguments: arguments
	end
end

# Configurations

define_configuration "development" do |configuration|
	configuration[:source] = "https://github.com/kurocha/"
	configuration.import "async"
	
	# Provides all the build related infrastructure:
	configuration.require "platforms"
	configuration.require "build-files"
	
	# Provides unit testing infrastructure and generators:
	configuration.require "unit-test"
	
	# Provides some useful C++ generators:
	configuration.require "generate-travis"
	configuration.require "generate-project"
	configuration.require "generate-cpp-class"
end

define_configuration "async" do |configuration|
	configuration.public!
	
	configuration.require "concurrent"
	configuration.require "parallel"
	configuration.require "time"
	configuration.require "memory"
end
