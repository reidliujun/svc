;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
; The following three entries are very important. Be sure to read and
; understand the '[LangOptions] section' topic in the help file.
LanguageName=<7B80><4F53><4E2D><6587>
LanguageID=$0804
LanguageCodePage=936
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
DialogFontName=宋体
DialogFontSize=9
WelcomeFontName=宋体
WelcomeFontSize=12
TitleFontName=宋体
TitleFontSize=29
CopyrightFontName=宋体
CopyrightFontSize=9

[Messages]

; *** Application titles
SetupAppTitle=安装
SetupWindowTitle=安装 - %1
UninstallAppTitle=卸载
UninstallAppFullTitle=卸载 %1

; *** Misc. common
InformationTitle=信息
ConfirmTitle=确认
ErrorTitle=错误

; *** SetupLdr messages
SetupLdrStartupMessage=即将安装 %1。您确定要继续吗？
LdrCannotCreateTemp=无法创建临时文件。安装终止
LdrCannotExecTemp=无法在临时文件文件夹运行一个文件。安装终止

; *** Startup error messages
LastErrorMessage=%1。%n%n错误 %2：%3
SetupFileMissing=安装文件夹中无法找到文件“%1”。请修正这个问题或者重新获取该安装程序完整的副本。
SetupFileCorrupt=安装文件已损坏。请重新获取该安装程序完整的副本。
SetupFileCorruptOrWrongVer=安装文件已损坏，或者与当前的安装程序版本不兼容。请修正这个问题或者重新获取该安装程序完整的副本。
NotOnThisPlatform=本程序不能运行在 %1 上。
OnlyOnThisPlatform=本程序必须运行在 %1 上。
OnlyOnTheseArchitectures=本程序只能安装在为以下处理器架构设计的 Windows 版本上：%n%n%1
MissingWOW64APIs=您运行的 Windows 版本不支持安装程序执行 64 位安装所需的功能。要修正这个问题，请安装 Service Pack %1。
WinVersionTooLowError=本程序需要 %1 版本 %2 或更高的版本。
WinVersionTooHighError=本程序不能安装在 %1 版本 %2 或更高的版本上。
AdminPrivilegesRequired=您必须以系统管理员的身份登录系统才能安装本程序。
PowerUserPrivilegesRequired=您必须以系统管理员的身份登录系统，或是您必须是 Power Users 组的成员，才能安装本程序。
SetupAppRunningError=安装程序检测到“%1”正在运行。%n%n请关闭其所有进程，然后单击“确定”以继续，或者单击“取消”以退出。
UninstallAppRunningError=卸载程序检测到“%1”正在运行。%n%n请关闭其所有进程，然后单击“确定”以继续，或者单击“取消”以退出。

; *** Misc. errors
ErrorCreatingDir=安装程序无法创建文件夹“%1”
ErrorTooManyFilesInDir=由于文件夹“%1”中包含了太多的文件，无法在其中创建新文件

; *** Setup common messages
ExitSetupTitle=退出安装
ExitSetupMessage=安装尚未完成。如果您现在退出，应用程序将没有被安装。%n%n您可以在其它时间重新运行这个安装程序以完成安装。%n%n您确定要退出安装程序吗？
AboutSetupMenuItem=关于安装程序(&A)...
AboutSetupTitle=关于安装程序
AboutSetupMessage=%1 版本 %2%n%3%n%n%1 网址：%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< 上一步(&B)
ButtonNext=下一步(&N) >
ButtonInstall=安装(&I)
ButtonOK=确定
ButtonCancel=取消
ButtonYes=是(&Y)
ButtonYesToAll=全是(&A)
ButtonNo=否(&N)
ButtonNoToAll=全否(&O)
ButtonFinish=完成(&F)
ButtonBrowse=浏览(&B)...
ButtonWizardBrowse=浏览(&R)...
ButtonNewFolder=创建新文件夹(&M)

; *** "Select Language" dialog messages
SelectLanguageTitle=选择安装语言
SelectLanguageLabel=请选择在安装过程中使用的语言：

; *** Common wizard text
ClickNext=单击“下一步”继续安装，单击“取消”退出安装。
BeveledLabel=
BrowseDialogTitle=浏览文件夹
BrowseDialogLabel=请在下面的列表中选择一个文件夹，然后单击“确定”。
NewFolderName=新建文件夹

; *** "Welcome" wizard page
WelcomeLabel1=欢迎使用“[name]”安装向导
WelcomeLabel2=即将在您的计算机上安装“[name/ver]”。%n%n建议您在继续安装之前关闭所有其它的应用程序。

; *** "Password" wizard page
WizardPassword=密码
PasswordLabel1=本安装受密码保护。
PasswordLabel3=请输入密码，单击“下一步”以继续安装。请注意密码是大小写敏感的。
PasswordEditLabel=密码(&P)：
IncorrectPassword=您输入的密码不正确。请再试一次。

; *** "License Agreement" wizard page
WizardLicense=许可协议
LicenseLabel=在继续安装之前，请阅读下面的重要信息。
LicenseLabel3=请阅读下面的许可协议。您必须同意许可协议上的条款，才可以继续安装。
LicenseAccepted=我同意许可协议上的条款(&A)
LicenseNotAccepted=我不同意许可协议上的条款(&D)

; *** "Information" wizard pages
WizardInfoBefore=信息
InfoBeforeLabel=在继续安装之前，请阅读下面的重要信息。
InfoBeforeClickLabel=当您准备好继续运行安装时，请单击“下一步”。
WizardInfoAfter=信息
InfoAfterLabel=在继续安装之前，请阅读下面的重要信息。
InfoAfterClickLabel=当您准备好继续运行安装时，请单击“下一步”。

; *** "User Information" wizard page
WizardUserInfo=用户信息
UserInfoDesc=请输入您的信息。
UserInfoName=用户姓名(&U)：
UserInfoOrg=组织(&O)：
UserInfoSerial=产品序列号(&S)：
UserInfoNameRequired=您必须输入用户姓名。

; *** "Select Destination Location" wizard page
WizardSelectDir=选择目标文件夹
SelectDirDesc=您打算将 [name] 安装在何处？
SelectDirLabel3=安装程序将把 [name] 安装到下面的文件夹。
SelectDirBrowseLabel=要继续安装，请单击“下一步”。如果你想要选择一个不同的文件夹，请单击“浏览”。
DiskSpaceMBLabel=至少需要 [mb] MB 的空余磁盘空间。
ToUNCPathname=安装程序不能将本程序安装到一个 UNC 路径中。如果您想在网络上安装，您需要首先映射网络驱动器。
InvalidPath=您必须输入包含驱动器的完整的路径，例如：%n%nC:\APP%n%n或者是如下形式的 UNC 路径：%n%n\\server\share
InvalidDrive=您输入的驱动器或 UNC 共享不存在或无法访问。请选择其它的驱动器或 UNC 共享。
DiskSpaceWarningTitle=磁盘空间不足
DiskSpaceWarning=安装程序需要至少 %1 KB 空余磁盘空间以进行安装，但是您选择的驱动器只剩下 %2 KB 的空间。%n%n您要强行继续安装吗？
DirNameTooLong=文件夹名称或路径名称太长。
InvalidDirName=无效的文件夹名称。
BadDirName32=文件夹名称中不能包含任意下面列出的字符：%n%n%1
DirExistsTitle=文件夹已存在
DirExists=文件夹：%n%n%1%n%n已存在。您仍然要安装到这个文件夹中吗？
DirDoesntExistTitle=文件夹不存在
DirDoesntExist=文件夹：%n%n%1%n%n不存在。您要创建这个文件夹吗？

; *** "Select Components" wizard page
WizardSelectComponents=选择组件
SelectComponentsDesc=您打算安装哪些组件？
SelectComponentsLabel2=请选择您准备安装的组件；清除您不准备安装的组件。当您准备继续安装时，请单击“下一步”。
FullInstallation=完整安装
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=典型安装
CustomInstallation=自定义安装
NoUninstallWarningTitle=组件已存在
NoUninstallWarning=安装程序检查到下列组件已经安装在您的计算机上：%n%n%1%n%n不选择这些组件将不会卸载它们。%n%n您要继续吗？
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=当前选择的组件需要至少 [mb] MB 空余磁盘空间。

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=选择附加安装任务
SelectTasksDesc=您需要安装程序执行哪些附加安装任务？
SelectTasksLabel2=请选择您在安装 [name] 时需要执行的附加安装任务，然后单击“下一步”。

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=选择开始菜单文件夹
SelectStartMenuFolderDesc=安装程序应在何处放置快捷方式？
SelectStartMenuFolderLabel3=安装程序将在下面的开始菜单文件夹中放置快捷方式。
SelectStartMenuFolderBrowseLabel=要继续安装，请单击“下一步”。要选择其它文件夹，请单击“浏览”。
MustEnterGroupName=您必须输入文件夹的名称。
GroupNameTooLong=文件夹名称或路径太长。
InvalidGroupName=无效的文件夹名称。
BadGroupName=文件夹名称中不能包含任意下面列出的字符：%n%n%1
NoProgramGroupCheck2=不要创建开始菜单文件夹(&D)

; *** "Ready to Install" wizard page
WizardReady=准备开始安装
ReadyLabel1=安装程序现在准备开始在您的计算机上安装 [name]。
ReadyLabel2a=请单击“安装”以继续进行安装，或者如果您打算复查或者修改某些设置，请单击“上一步”。
ReadyLabel2b=请单击“安装”以继续进行安装
ReadyMemoUserInfo=用户信息：
ReadyMemoDir=目标文件夹：
ReadyMemoType=安装类型：
ReadyMemoComponents=选择的组件：
ReadyMemoGroup=开始菜单文件夹：
ReadyMemoTasks=附加安装任务：

; *** "Preparing to Install" wizard page
WizardPreparing=安装准备
PreparingDesc=安装程序正在为将 [name] 安装到您的计算机上进行准备。
PreviousInstallNotCompleted=上一次程序的安装/删除操作尚未完成。您需要重新启动计算机，并再次运行本安装程序以完成 [name] 的安装。
CannotContinue=安装程序无法继续。请单击“取消”以退出。

; *** "Installing" wizard page
WizardInstalling=正在安装
InstallingLabel=安装程序正在将 [name] 安装到您的计算机中，请稍候。

; *** "Setup Completed" wizard page
FinishedHeadingLabel=完成 [name] 安装向导
FinishedLabelNoIcons=安装程序已经将 [name] 安装到您的计算机上。
FinishedLabel=安装程序已经将 [name] 安装到您的计算机上。您可以通过已经安装的快捷方式图标来启动应用程序。
ClickFinish=请单击“完成”退出安装程序。
FinishedRestartLabel=要完成 [name] 的安装，安装程序必须重新启动您的计算机。您准备现在就重新启动吗？
FinishedRestartMessage=要完成 [name] 的安装，安装程序必须重新启动您的计算机。%n%n您准备现在就重新启动吗？
ShowReadmeCheck=是的，我准备查看自述文件
YesRadio=是，现在就重新启动计算机(&Y)
NoRadio=否，我准备稍后重新启动计算机(&N)
; used for example as 'Run MyProg.exe'
RunEntryExec=运行“%1”
; used for example as 'View Readme.txt'
RunEntryShellExec=查看“%1”

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=安装程序需要下一个安装介质
SelectDiskLabel2=请插入安装介质 %1 并单击“确定”。%n%n如果该安装介质上的文件可以在下面列出的路径以外获得，请输入正确的路径或单击“浏览”。
PathLabel=路径(&P)：
FileNotInDir2=无法在“%2”中获取文件“%1”。请插入正确的安装介质或选择其它文件夹。
SelectDirectoryLabel=请指定下一个安装介质的位置。

; *** Installation phase messages
SetupAborted=安装工作尚未完成。%n%n请修正出现的问题并再次运行安装程序。
EntryAbortRetryIgnore=单击“重试”再尝试一次，单击“忽略”强制继续，或者单击“终止”取消安装。

; *** Installation status messages
StatusCreateDirs=正在创建文件夹……
StatusExtractFiles=正在解压缩文件……
StatusCreateIcons=正在创建快捷方式……
StatusCreateIniEntries=正在创建 INI 项目……
StatusCreateRegistryEntries=正在创建注册表项目……
StatusRegisterFiles=正在注册文件……
StatusSavingUninstall=正在保存卸载信息……
StatusRunProgram=正在完成安装……
StatusRollback=正在还原本次安装所做的改动……

; *** Misc. errors
ErrorInternal2=内部错误：%1
ErrorFunctionFailedNoCode=%1失败
ErrorFunctionFailed=%1失败；错误编码：%2
ErrorFunctionFailedWithMessage=%1失败；错误编码：%2。%n%3
ErrorExecutingProgram=无法运行文件：%n%1

; *** Registry errors
ErrorRegOpenKey=打开注册表主键时产生错误：%n%1\%2
ErrorRegCreateKey=创建注册表主键时产生错误：%n%1\%2
ErrorRegWriteKey=写入注册表键值时产生错误：%n%1\%2

; *** INI errors
ErrorIniEntry=在文件“%1”中创建 INI 项目时发生错误。

; *** File copying errors
FileAbortRetryIgnore=单击“重试”再尝试一次，单击“忽略”跳过此文件(不推荐)，或者单击“终止”取消安装。
FileAbortRetryIgnore2=单击“重试”再尝试一次，单击“忽略”强制继续(不推荐)，或者单击“终止”取消安装。
SourceIsCorrupted=原始文件损坏
SourceDoesntExist=原始文件“%1”不存在
ExistingFileReadOnly=现有文件是只读的。%n%n单击“重试”以删除只读属性并再尝试一次，单击“忽略”跳过此文件，或者单击“终止”取消安装。
ErrorReadingExistingDest=尝试读取现有文件时产生错误：
FileExists=该文件已存在。%n%n您需要安装程序覆盖它吗？
ExistingFileNewer=现有文件比本安装程序尝试安装的版本更新。建议您保留现有的文件。%n%n您要保留现有文件吗？
ErrorChangingAttr=尝试修改现有文件属性时产生错误：
ErrorCreatingTemp=尝试在目标文件夹中创建文件时产生错误：
ErrorReadingSource=尝试读取原始文件时产生错误：
ErrorCopying=尝试复制文件时产生错误：
ErrorReplacingExistingFile=尝试替换现有文件时产生错误：
ErrorRestartReplace=重启覆盖失败：
ErrorRenamingTemp=尝试在目标文件夹中更名文件时产生错误：
ErrorRegisterServer=无法注册 DLL/OCX：%1
ErrorRegSvr32Failed=RegSvr32 运行失败，返回码为：%1
ErrorRegisterTypeLib=无法注册类型库：%1

; *** Post-installation errors
ErrorOpeningReadme=尝试打开自述文件时产生错误。
ErrorRestartingComputer=安装程序无法重新启动计算机。请手工重启。

; *** Uninstaller messages
UninstallNotFound=文件“%1”不存在。无法执行卸载操作。
UninstallOpenError=文件“%1”无法打开。无法执行卸载操作。
UninstallUnsupportedVer=当前版本的卸载程序无法识别卸载日志文件“%1”的格式。无法执行卸载操作。
UninstallUnknownEntry=卸载日志文件存在未知项目(%1)
ConfirmUninstall=您确定要完全卸载 %1 和它的所有组件吗？
UninstallOnlyOnWin64=该安装只能在 64 位 Windows 系统上卸载。
OnlyAdminCanUninstall=该安装只能由具有系统管理员权限的用户卸载。
UninstallStatusLabel=正在从您的计算机上卸载 %1，请稍候。
UninstalledAll= %1 已经成功从您的计算机上卸载。
UninstalledMost= %1 卸载完成。%n%n一些对象没有被删除。您可以手工删除它们。
UninstalledAndNeedsRestart=要完成 %1 的卸载，需要重新启动您的计算机。%n%n您准备现在就重新启动吗？
UninstallDataCorrupted=文件“%1”损坏。无法执行卸载操作。

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=删除共享文件？
ConfirmDeleteSharedFile2=系统显示以下共享文件将不再被任何程序使用。您打算卸载这些共享文件吗？%n%n如果其它程序仍然需要使用这个文件而它被删除了，那些程序将可能无法运行。如果您不确定，请选择“否”。在您的系统中保留这些文件不会带来任何伤害。
SharedFileNameLabel=文件名：
SharedFileLocationLabel=位置：
WizardUninstalling=卸载状态
StatusUninstalling=正在卸载 %1……

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 版本 %2
AdditionalIcons=附加图标：
CreateDesktopIcon=创建桌面图标(&D)
CreateQuickLaunchIcon=创建快速启动图标(&Q)
ProgramOnTheWeb=网络上的“%1”
UninstallProgram=卸载“%1”
LaunchProgram=运行“%1”
AssocFileExtension=将扩展名为“%2”的文件关联到“%1”(&A)
AssocingFileExtension=正在将扩展名为“%2”的文件关联到“%1”……
