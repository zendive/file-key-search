#pragma once

#define sPRG_COMPANY_NAME     _T("Noosphere")
#define sPRG_APP_NAME         _T("FileKeySearch")
#define sPRG_APP_RESULT_HEADER  _T("FILEKEYSEARCH::RESULT")
// App major version. Used in registry path building
#define sPRG_MAJOR_VERSION    _T("2")
// ... and serialization
#define nPRG_MAJOR_VERSION    (2)
// App version string
#define sPRG_VERSION          _T("2.1")
// App document extension
#define sPRG_EXTENSION        _T(".") sPRG_APP_NAME
// App d
#define sPRG_KEY_EXPORT_DEFAULT_NAME  sPRG_APP_NAME _T(".Keys.REG")

// Max path string len.
#define nPRG_MAX_PATH               (1024)  // ?

// Max Key-Signature size in bytes.
#define nPRG_MAX_KEYSIGNATURE_SIZE  (1024)

// Max Key-Comment size in wchar_t ('\0' included).
#define nPRG_MAX_KEYCOMMENT_SIZE    (513)

// Max Key-Name size in wchar_t ('\0' included).
#define nPRG_MAX_KEYNAME_SIZE       (22)
#define nPRG_MAX_MEDIANAME_SIZE     (32)

// Max Key's count that app can handle.
#define nPRG_MAX_KEY_COUNT          (512)
#define sPRG_MAX_KEY_COUNT_MESSAGE  _T("This version can't handle more keys than it has now.")
#define sPRG_MAX_KEY_COUNT_MESSAGE2 _T("Opened file has more keys than this version can handle.")

// Max Key-Extension size in wchar_t ('\0' included).
#define nPRG_MAX_KEYEXTENSION_SIZE  (60)

// Max Error message buffer size  (used in CApp::Report).
#define nPRG_MAX_ERRORMESSAGE_SIZE  (1025)

// Items count limit. This limit shows how mach items can be diplayed
// in list-control-window without a display deffects & crashes
// 12000 on WinXP; 99999 on Win7
#define nPRG_MAX_ITEMS_LIMIT        (99999)//16384

// Max number of documents to store/restore while closing/starting the app
#define nPRG_MAX_LASTOPENED_REMEMBERED (1000)

#define sPRG_REGPATH_APP            _T("SOFTWARE\\") sPRG_COMPANY_NAME _T("\\") sPRG_APP_NAME
// Path to Application Version in the Registry.
#define sPRG_REGPATH_VER            sPRG_REGPATH_APP _T("\\") sPRG_MAJOR_VERSION

// Path in the registry to the Keys location (programm version included).
#define sPRG_REGNAME_KEYS           _T("Keys")
#define sPRG_REGPATH_KEYS           sPRG_REGPATH_VER _T("\\") sPRG_REGNAME_KEYS

// Path to last opened documents list
#define sPRG_REGNAME_LASTOPENED     _T("Last File List")
#define sPRG_REGPATH_LASTOPENED     sPRG_REGPATH_APP _T("\\") sPRG_REGNAME_LASTOPENED

// The Report Dialog caption.
#define sPRG_REPORTDLG_CAPTION      _T(" Report")
// The Export Dialog caption.
#define sPRG_EXPORTDLG_CAPTION      _T(" Export Keys")
// The Import Dialog caption.
#define sPRG_IMPORTDLG_CAPTION      _T(" Import Keys")
// The Media Properties caption.
#define sPRG_MEDIADLG_CAPTION       _T(" Media Properties")

// The Keys Manager Dialog caption.
#define sPRG_KEYMANAGERDLG_GLOBALCAPTION  _T(" Global Keys Manager")
#define sPRG_KEYMANAGERDLG_LOCALCAPTION   _T(" Local Keys Manager")
#define sPRG_KEYMANAGERDLG_KEYLISTTITLE   _T("Key Collection")
// The Keys New-Name/Rename Dialog.
#define sPRG_NEWNAMEDLG_CAPTION     _T(" Name Assign")
// The Media New-Extension Dialog.
#define sPRG_NEWEXTDLG_CAPTION      _T(" Extension Assign")
// The DialogBox caption
#define sPRG_WARNING_CAPTION        _T(" WARNING")
// The DialogBox caption
#define sPRG_NAMECONFLICT_CAPTION   _T(" Key name conflict")

// The Keys Update caption when updating from Analyze Data File
#define sPRG_KEYUPDATE_CAPTION      _T(" Import New Keys?")

// Strings for Import Type.
#define sPRG_IMPORTTYPE_REPLACE_THE_SAME  _T("Overwrite existing")
#define sPRG_IMPORTTYPE_SKIP_THE_SAME     _T("Skip existing")
#define sPRG_IMPORTTYPE_IMPORT_ON_CLEAR   _T("Remove All and Import on Clear")

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Const names of registry entries.
#define sPRG_KEY_COMMENT            _T("Comment")
#define sPRG_KEY_SIGNATURE          _T("Signature")
#define sPRG_KEY_SIGNSIZE           _T("Size")
#define sPRG_KEY_CSO                _T("CSO")
#define sPRG_KEY_CSOTYPE            _T("CSOType")
#define sPRG_KEY_SKIPSCOPE          _T("SkipScope")
#define sPRG_KEY_EXTENSIONS         _T("Extensions")

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Const string delimeters and badchars mappings
#define sPRG_KEY_SIGNATURE_DELIMETERS _T(".,:;+-`'\"*[]<>()#&\t\x20\xA0\x0D\x0A")
#define sPRG_MEDIA_FILENAME_BADCHARS  _T("*?\"/\\<>")
#define sPRG_KEY_EXTENSION_BADCHARS   sPRG_MEDIA_FILENAME_BADCHARS _T("`.,'")

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Const tree root captions
#define sPRG_KEYTREE_FOUND          _T("Total Found")
#define sPRG_KEYTREE_NOTFOUND       _T("Not Found")

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Template values for new key.
#define sPRG_KEYTEMPLATE_NAME       _T("NewKey")
#define sPRG_KEYTEMPLATE_COMMENT    _T("Comment for new Key")
#define sPRG_KEYTEMPLATE_SIGNATURE  {'\xAC', '\xDC'}
#define nPRG_KEYTEMPLATE_SIGNSIZE   (2)
#define nPRG_KEYTEMPLATE_CSO        (0)
#define nPRG_KEYTEMPLATE_CSOTYPE    CKey::CSOTYPE_UNASSIGNED
#define nPRG_KEYTEMPLATE_SKIPSCOPE  (0)
#define sPRG_KEYTEMPLATE_EXTENSIONS _T("bin;hex")

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// [ PExTA ANALYZE PARAMETERS ]
// Memory allocation configuration parameters to self-adjust search buffer size
// to achieve high search performance in bounds of free memory
#define nPRG_ANALYZE_MEMORY_SOS     (1000)      // 1 KB
#define nPRG_ANALYZE_MEMORY_LOW     (4000)      // 4 KB
#define nPRG_ANALYZE_MEMORY_NORMAL  (2000000)   // 2 MB [mimic HD cashe size]
#define nPRG_ANALYZE_MEMORY_HIGH    (10000000)  // 10 MB [optimal for work with RAM]
// buffer used in the extraction process
#define nPRG_EXTRACT_BUFFER_SIZE    (1048576)   // 1 MiB

// Amount of time and number of retries before retring after malloc have failed to allocate
//  buffer for next file portion
#define nPRG_ANALTHREAD_RETRYONMALLOCFAIL_SLEEP_MS      (5120)
#define nPRG_ANALTHREAD_RETRYONMALLOCFAIL_TRIES_NUMBER  (4)
#define nPRG_THREAD_WHILE_ONPAUSE_SLEEP_MS              (500)

// Amount of time in msec to waite before killing freezed analyze thread (SOS case)
#define nPRG_ANALTHREAD_WAIT2KILL_MS  (16 * nPRG_ANALTHREAD_REPAUSE_SLEEP_MS)

// Memory buffer size to allocate/free/allocate between CApp::Report dialogs
#define nPRG_APP_REPORT_MEMORY_STUB_AMOUNT  (4096)

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Post message to Analyze thread to start analyze.
#define nPRG_WM_ANALYZETHREAD_START    (WM_USER + 1)
#define nPRG_WM_EXTRACTTHREAD_START    (WM_USER + 2)
