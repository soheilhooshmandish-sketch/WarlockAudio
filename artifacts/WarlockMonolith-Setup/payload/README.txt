Drop the Release VST3 bundle here as:

  payload/WarlockMonolith.vst3/Contents/...

The Inno script copies this folder into Common Files\VST3.
The installer still compiles if the bundle is missing (skipifsourcedoesntexist)
so the license pages can be tested before the DSP build is ready.
