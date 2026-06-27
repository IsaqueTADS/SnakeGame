const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const exe = b.addExecutable(.{
        .name = "chip8",
        .root_module = exe_mod,
    });
    exe.addIncludePath(b.path("vendor/raylib/src"));
    exe.addIncludePath(b.path("vendor/raylib/src/external/glfw/include"));

    const raylib_sources = &.{
        "src/main.c",
        "src/menu.c",
        "src/snake.c",
        "src/heart.c",
        "src/audio.c",
        "src/music.c",
        "vendor/raylib/src/rcore.c",
        "vendor/raylib/src/rshapes.c",
        "vendor/raylib/src/rtextures.c",
        "vendor/raylib/src/rtext.c",
    };

    const glfw_sources: []const []const u8 = switch (target.result.os.tag) {
        .linux => &.{
            "vendor/raylib/src/external/glfw/src/context.c",
            "vendor/raylib/src/external/glfw/src/init.c",
            "vendor/raylib/src/external/glfw/src/input.c",
            "vendor/raylib/src/external/glfw/src/monitor.c",
            "vendor/raylib/src/external/glfw/src/platform.c",
            "vendor/raylib/src/external/glfw/src/vulkan.c",
            "vendor/raylib/src/external/glfw/src/window.c",
            "vendor/raylib/src/external/glfw/src/x11_init.c",
            "vendor/raylib/src/external/glfw/src/x11_monitor.c",
            "vendor/raylib/src/external/glfw/src/x11_window.c",
            "vendor/raylib/src/external/glfw/src/glx_context.c",
            "vendor/raylib/src/external/glfw/src/linux_joystick.c",
            "vendor/raylib/src/external/glfw/src/posix_module.c",
            "vendor/raylib/src/external/glfw/src/posix_time.c",
            "vendor/raylib/src/external/glfw/src/posix_thread.c",
            "vendor/raylib/src/external/glfw/src/posix_poll.c",
            "vendor/raylib/src/external/glfw/src/xkb_unicode.c",
            "vendor/raylib/src/external/glfw/src/egl_context.c",
            "vendor/raylib/src/external/glfw/src/osmesa_context.c",
            "vendor/raylib/src/raudio.c",
        },
        .windows => &.{
            "vendor/raylib/src/external/glfw/src/context.c",
            "vendor/raylib/src/external/glfw/src/init.c",
            "vendor/raylib/src/external/glfw/src/input.c",
            "vendor/raylib/src/external/glfw/src/monitor.c",
            "vendor/raylib/src/external/glfw/src/platform.c",
            "vendor/raylib/src/external/glfw/src/vulkan.c",
            "vendor/raylib/src/external/glfw/src/window.c",
            "vendor/raylib/src/external/glfw/src/win32_module.c",
            "vendor/raylib/src/external/glfw/src/win32_init.c",
            "vendor/raylib/src/external/glfw/src/win32_joystick.c",
            "vendor/raylib/src/external/glfw/src/win32_monitor.c",
            "vendor/raylib/src/external/glfw/src/win32_time.c",
            "vendor/raylib/src/external/glfw/src/win32_thread.c",
            "vendor/raylib/src/external/glfw/src/win32_window.c",
            "vendor/raylib/src/external/glfw/src/wgl_context.c",
            "vendor/raylib/src/external/glfw/src/egl_context.c",
            "vendor/raylib/src/external/glfw/src/osmesa_context.c",
        },
        .macos => &.{
            "vendor/raylib/src/external/glfw/src/context.c",
            "vendor/raylib/src/external/glfw/src/init.c",
            "vendor/raylib/src/external/glfw/src/input.c",
            "vendor/raylib/src/external/glfw/src/monitor.c",
            "vendor/raylib/src/external/glfw/src/platform.c",
            "vendor/raylib/src/external/glfw/src/vulkan.c",
            "vendor/raylib/src/external/glfw/src/window.c",
            "vendor/raylib/src/external/glfw/src/cocoa_time.c",
            "vendor/raylib/src/external/glfw/src/egl_context.c",
            "vendor/raylib/src/external/glfw/src/osmesa_context.c",
        },
        else => @panic("unsupported OS tag"),
    };

    const cflags: []const []const u8 = switch (target.result.os.tag) {
        .linux => &.{
            "-std=c99",
            "-D_DEFAULT_SOURCE",
            "-DPLATFORM_DESKTOP",
            "-DGRAPHICS_API_OPENGL_33",
            "-D_GLFW_X11",
        },
        .windows => &.{
            "-std=c99",
            "-DPLATFORM_DESKTOP",
            "-DGRAPHICS_API_OPENGL_33",
            "-D_GLFW_WIN32",
        },
        .macos => &.{
            "-std=c99",
            "-DPLATFORM_DESKTOP",
            "-DGRAPHICS_API_OPENGL_33",
            "-D_GLFW_COCOA",
        },
        else => @panic("unsupported OS tag"),
    };

    exe.addCSourceFiles(.{ .files = raylib_sources, .flags = cflags });
    exe.addCSourceFiles(.{ .files = glfw_sources, .flags = cflags });

    exe.linkLibC();

    switch (target.result.os.tag) {
        .linux => {
            exe.linkSystemLibrary("GL");
            exe.linkSystemLibrary("m");
            exe.linkSystemLibrary("pthread");
            exe.linkSystemLibrary("dl");
            exe.linkSystemLibrary("rt");
            exe.linkSystemLibrary("X11");
        },
        .macos => {
            exe.linkFramework("OpenGL");
            exe.linkFramework("Cocoa");
            exe.linkFramework("IOKit");
            exe.linkFramework("CoreVideo");
        },
        .windows => {
            exe.linkSystemLibrary("opengl32");
            exe.linkSystemLibrary("gdi32");
            exe.linkSystemLibrary("winmm");
        },
        else => {},
    }

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run CHIP-8 emulator");
    run_step.dependOn(&run_cmd.step);
}
