# file-key-search
Search and extract by file signature

![screenshot](./doc/screenshot-globalKeysManagerDialog.png)

#### About

This Programm provides ability to search the target file for the embedded files by the specific format parameters predefined as "Keys" disregarding the rules of such file packaging.

Each Key represents minimum file format specification that allows to distinguish it from other file formats during the Search.

It is suitable to find and extract recognized items (Media) in a file that is known to combine other file-types inside. For example that may be a raw uncompressed hard-disk/memory dump (image) or a data-file that accumulates other resources that are used (indirectly) by the data-file owner programm.

#### Requirements
Operating System: Win 2000 or later; Linux (Ubuntu, openSUSE) with "wine" 1.0 or later.
Optionally: Hex Viewer/Editor in order to find and use File Signature to create a search-key (HxD for example).

#### Installation

   Extract the archive to the location of your will, for example "C:\Program Files\FileKeySearch\";
   To load a preset of keys that comes whith the programm archive:

   execute FileKeySearch.exe then select from main menu: Keys->Import from File...->Open File..; Browse for "C:\Program Files\FileKeySearch\FileKeySearch.Keys.REG"; click Import and Close. Following file signatures will be imported into the Registry:
            RAR, ZIP, ELF, MZ, HTML, XML, PDF, BMP, JFiF, EXIF, GIF, JNG, MNG, PNG, MP3, OggS, BiK, FLV, RiFF (wav/avi)

   You can add more keys later in Keys->Keys Manager and even export to .REG file for a backup/sharing purposes. Furthermore, this site may come in help if youre looking for a rare/new signature definition: File Signatures Table

#### Usage

   To start file analysis choose File->New, then Browse for the target file or drag-in that file with your mouse, after clicking Next select which Keys you want to be used during the search*, proceed to the Finish to start analysis. After analysis, programm will display Local Key tree on the left pane and table of found Media items on the right pane (if any was found). Click on some key under Total Found tree-root to see found items under that key. To extract found media select them and choose Extract from the right-click-menu. You can save search results with Local Keys and all metadata about found items in a file with ```*.FileKeySearch``` extension.

   * Note that keys used for analyze once they selected called "Local Keys" become logically separated from the programm keys called "Global Keys", and can be changed by double clicking on any key in a key-tree.

#### Uninstall

   To uninstall simply remove programm's folder, then run "regedit.exe" and remove this key that stores file signature metadata: HKEY_CURRENT_USER\Software\Noosphere\FileKeySearch
