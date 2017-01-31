{
  'targets': [
    {
      "variables": {
        "ad_da_verbose%": "false"
      },
      "target_name": "npm_demo",
      "sources": [ "npm_demo.cpp", "ad-da.cpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [ "-lbcm2835" ],
      "conditions": [
        ["OS=='linux'", {
          "cflags": [ "-std=c++11" ],
          "include_dirs+": "/usr/local/lib/libbcm2835.a",
          "sources": ["npm_demo.cpp", "ad-da.cpp" ]
        }],
        ["ad_da_verbose=='true'", {
          "defines": [ "VERBOSE" ]
        }]
      ]
    }
  ]
}
