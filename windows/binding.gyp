{
  "targets": [
    {
      "target_name": "screen_capture",
      "sources": [ "capture/screen.cpp", "capture/SimpleCapture.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "$(WindowsSdkDir)Include\\$(WindowsTargetPlatformVersion)\\cppwinrt",
        "$(WindowsSdkDir)Include\\$(WindowsTargetPlatformVersion)\\um",
        "$(WindowsSdkDir)Include\\$(WindowsTargetPlatformVersion)\\shared"
      ],
      "library_dirs": [
        "$(WindowsSdkDir)Lib\\$(WindowsTargetPlatformVersion)\\um\\x64"
      ],
      "libraries": [
        "d3d11.lib", 
        "dxgi.lib", 
        "windowsapp.lib"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1,
          "AdditionalOptions": ["/std:c++17", "/await", "/EHsc", "/Zc:twoPhase-", "/bigobj"],
          "PreprocessorDefinitions": ["WIN32_LEAN_AND_MEAN", "NOMINMAX"]
        },
        "VCLinkerTool": {
          "GenerateFullProgramDatabaseFile": "true"
        }
      },
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ]
    }
  ]
}