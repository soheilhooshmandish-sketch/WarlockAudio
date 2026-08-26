Steinberg Cubase / Nuendo Media Rack snapshots (VST 3.6.10+).

MUST live at:
  Warlock.vst3/Contents/Resources/Snapshots/

MUST be PNG. MUST be named:

  {CID32HEX}_snapshot.png
  {CID32HEX}_snapshot_2.0x.png

CID is the VST3 processor TUID, 32 hex chars, no braces or dashes.
JUCE derives it from manufacturer Nhil + plugin Wrck on first build.

Until you rename these files to the real CID, hosts still load the plug-in;
only the rack thumbnail is missing.

After a Release build, open
  build/Warlock_artefacts/Release/VST3/Warlock.vst3/Contents/Resources/moduleinfo.json
(if JUCE wrote one) and copy Classes[0].CID (strip punctuation) into the filenames.

Warlock_snapshot.png      = 1x
Warlock_snapshot_2.0x.png = HiDPI
