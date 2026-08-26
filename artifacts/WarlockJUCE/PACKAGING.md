# VST3 packaging — small bundle

Skins and IRs compile **once** into the DLL (`juce_add_binary_data`).  
`Contents/Resources` only holds what a host reads *without* loading code: snapshots, legal, moduleinfo.

| Layer | What’s in it | ~size |
|---|---|---|
| BinaryData (DLL) | chassis, knob, stomp, logo, 3 IRs | ~105 KB |
| Contents/Resources | snapshots 1x + 2.0x, legal, moduleinfo | ~161 KB |
| Plugin.ico | Explorer glyph | < 1 KB |

Do **not** copy chassis/knobs/IRs next to the DLL — that doubled the install for no UI gain.

Images are JPEG q72 / 192–384 px, logo 192 px PNG, snapshots 48-color PNG.
