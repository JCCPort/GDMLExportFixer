A little script to fix exported GDML files to be readable by Geant4

Needs boost as a dependency.

Run like: `./GDMLExportFixer inputfile.gdml outputfile.gdml`

Note that you will have to manually define the center point and also a world volume, for example:
`<position name="center" x="0.0" y="0.0" z="0.0" unit="mm"/>`

`<box aunit="radian" lunit="mm" name="worldbox" x="10000" y="10000" z="10000" />`

The first line needs to be added to the `define` section and the second to the `solids` section.
