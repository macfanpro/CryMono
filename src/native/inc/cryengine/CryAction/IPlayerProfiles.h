#include DEVIRTUALIZE_HEADER_FIX(IPlayerProfiles.h)

#ifndef __IPLAYERPROFILES_H__
#define __IPLAYERPROFILES_H__

#if _MSC_VER > 1000
#	pragma once
#endif

#include <IFlowSystem.h>
#include "CheatProtection.h"

struct IPlayerProfile;
struct IActionMap;
struct ISaveGame;
struct ILoadGame;

UNIQUE_IFACE struct ISaveGameThumbnail
{
	virtual	~ISaveGameThumbnail(){}
	// a thumbnail is a image in BGR or BGRA format
	// uint8* p; p[0] = B; p[1] = G; p[2] = R; p[3] = A;

	virtual const char* GetSaveGameName() = 0;
	// image access
	virtual void  GetImageInfo(int& width, int& height, int& depth) = 0;
	virtual int   GetWidth() = 0;
	virtual int   GetHeight() = 0;
	virtual int   GetDepth() = 0;
	virtual const uint8* GetImageData() = 0;

	// smart ptr
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<ISaveGameThumbnail> ISaveGameThumbailPtr;

UNIQUE_IFACE struct ISaveGameEnumerator
{
	virtual ~ISaveGameEnumerator(){}
	struct SGameMetaData
	{
		const char* levelName;
		const char* gameRules;
		int         fileVersion;
		const char* buildVersion;
		time_t      saveTime;
		time_t			loadTime;
		XmlNodeRef  xmlMetaDataNode;
	};

	struct SGameDescription
	{
		const char* name;
		const char* humanName;
		const char* description;
		SGameMetaData metaData;
	};

	virtual int  GetCount() = 0;
	virtual bool GetDescription(int index, SGameDescription& desc) = 0;

	// Get thumbnail (by index or save game name)
	virtual ISaveGameThumbailPtr GetThumbnail(int index) = 0;
	virtual ISaveGameThumbailPtr GetThumbnail(const char* saveGameName) = 0;

	// smart ptr
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<ISaveGameEnumerator> ISaveGameEnumeratorPtr;

UNIQUE_IFACE struct IAttributeEnumerator
{
	virtual ~IAttributeEnumerator(){}
	struct SAttributeDescription
	{
		const char* name;
	};

	virtual bool Next(SAttributeDescription& desc) = 0;

	// smart ptr
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<IAttributeEnumerator> IAttributeEnumeratorPtr;

enum EProfileReasons // bitfield
{
	ePR_Game				= 0x01,		// saving/loading a game/checkpoint
	ePR_Options			= 0x02,		// saving/loading profile options only
	ePR_All					= 0xff		// all flags
};

class IPlayerProfileListener
{
public:
	virtual ~IPlayerProfileListener(){}
	virtual void SaveToProfile(IPlayerProfile* pProfile, bool online, unsigned int /*EProfileReasons*/ reason) = 0;
	virtual void LoadFromProfile(IPlayerProfile* pProfile, bool online, unsigned int /*EProfileReasons*/ reason) = 0;
};

class IOnlineAttributesListener
{
public:
	enum EEvent
	{
		eOAE_Invalid = -1,
		eOAE_Register = 0,
		eOAE_Load,
		eOAE_Max,
	};

	enum EState
	{
		eOAS_None,
		eOAS_Processing,
		eOAS_Failed,
		eOAS_Success,
	};
	virtual ~IOnlineAttributesListener(){}
	virtual void OnlineAttributeState(EEvent event, EState newState) = 0;
};

#define INVALID_CONTROLLER_INDEX 0xFFFFFFFF

UNIQUE_IFACE struct IPlayerProfileManager
{
	struct SProfileDescription
	{
		const char* name;
		time_t lastLoginTime;
		SProfileDescription():lastLoginTime(NULL)
		{}
	};

	struct SUserInfo
	{
		const char* userId;
	};
	virtual ~IPlayerProfileManager(){}
	virtual bool Initialize() = 0;
	virtual bool Shutdown() = 0;

	virtual void GetMemoryUsage( ICrySizer * ) const = 0;

	// win32:    currently logged on user
	// xbox360:  more than one user can be signed in
	// ps3:      several connected pads possible

	// login the user
	virtual int  GetUserCount() = 0;
	virtual bool GetUserInfo(int index, IPlayerProfileManager::SUserInfo& outInfo) = 0;
	virtual bool LoginUser(const char* userId, bool& bOutFirstTime) = 0;
	virtual bool LogoutUser(const char* userId) = 0;

	virtual int  GetProfileCount(const char* userId) = 0;
	virtual bool GetProfileInfo(const char* userId, int index, IPlayerProfileManager::SProfileDescription& outInfo) = 0;
	virtual void SetProfileLastLoginTime(const char* userId, int index, time_t lastLoginTime) = 0;

	enum EProfileOperationResult
	{
		ePOR_Success					= 0,
		ePOR_NotInitialized		= 1,
		ePOR_NameInUse				= 2,
		ePOR_UserNotLoggedIn	= 3,
		ePOR_NoSuchProfile		= 4,
		ePOR_ProfileInUse			= 5,
		ePOR_NoActiveProfile	= 6,
		ePOR_DefaultProfile   = 7,
		ePOR_Unknown					=	255,
	};

	// create a new profile for a user 
	virtual bool CreateProfile(const char* userId, const char* profileName, bool bOverrideIfPresent, EProfileOperationResult& result) = 0;

	// delete a profile of an user 
	virtual bool DeleteProfile(const char* userId, const char* profileName, EProfileOperationResult& result) = 0;

	// rename the current profile of the user
	virtual bool RenameProfile(const char* userId, const char* newName, EProfileOperationResult& result) = 0;

	// save a profile
	virtual bool SaveProfile(const char* userId, EProfileOperationResult& result, unsigned int reason) = 0;

	// load and activate a profile, returns the IPlayerProfile if successful
	virtual IPlayerProfile* ActivateProfile(const char* userId, const char* profileName) = 0;

	// get the current profile of the user
	virtual IPlayerProfile* GetCurrentProfile(const char* userId) = 0;

	// get the current user
	virtual const char* GetCurrentUser() = 0;

	// get the current user index
	virtual int GetCurrentUserIndex() = 0;

	// set the controller index
	virtual void SetExclusiveControllerDeviceIndex(unsigned int exclusiveDeviceIndex) = 0;

	// get the controller index
	virtual unsigned int GetExclusiveControllerDeviceIndex() const = 0;

	// reset the current profile
	// reset actionmaps and attributes, don't delete save games! 
	virtual bool ResetProfile(const char* userId) = 0;

	// reload profile by calling each IPlayerProfileListener::LoadFromProfile
	virtual void ReloadProfile(IPlayerProfile* pProfile, unsigned int reason) = 0;

	// get the (always present) default profile (factory default)
	virtual IPlayerProfile* GetDefaultProfile() = 0;

	// load a profile for previewing only. there is exactly one preview profile for a user
	// subsequent calls will invalidate former profiles. profileName can be "" or NULL which will
	// delete the preview profile from memory
	virtual const IPlayerProfile* PreviewProfile(const char* userId, const char* profileName) = 0;

	// Set a shared savegame folder for all profiles
	// this means all savegames get prefixed with the profilename and '_'
	// by default: SaveGame folder is shared "%USER%/SaveGames/"
	virtual void SetSharedSaveGameFolder(const char* sharedSaveGameFolder) = 0;

	// Get the shared savegame folder
	virtual const char* GetSharedSaveGameFolder() = 0;

	// Register a listener to receive calls for online profile events
	virtual void AddListener(IPlayerProfileListener* pListener, bool updateNow) = 0;

	// Stop a listener from recieving events once registered
	virtual void RemoveListener(IPlayerProfileListener* pListener) = 0;

	// Register a listener to receive calls for online profile events
	virtual void AddOnlineAttributesListener(IOnlineAttributesListener* pListener) = 0;

	// Stop a listener from recieving events once registered
	virtual void RemoveOnlineAttributesListener(IOnlineAttributesListener* pListener) = 0;

	//enable online attribute registation/loading/saving
	virtual void EnableOnlineAttributes(bool enable) = 0;

	// if saving and loading online has been enabled
	virtual bool HasEnabledOnlineAttributes() const = 0;

	// if saving and loading online is allowed
	virtual bool CanProcessOnlineAttributes() const = 0;

	//enable online attribute processing - disables saving and loading whilst not in a valid state e.g. in a mp game session that's in progress
	virtual void SetCanProcessOnlineAttributes(bool enable) = 0;

	//register online attributes (needs to be done after online services are enabled)
	virtual bool RegisterOnlineAttributes() = 0;

	//Get the current state of the online attributes
	virtual void GetOnlineAttributesState(const IOnlineAttributesListener::EEvent event, IOnlineAttributesListener::EState &state) const = 0;

	// load profile's online attributes
	virtual void LoadOnlineAttributes(IPlayerProfile* pProfile) = 0;

	// set the online attributes from an outside source
	virtual void SetOnlineAttributes(IPlayerProfile *pProfile, const SCryLobbyUserData *pData, const int32 onlineDataCount) = 0;

	// retrieve the current online attributes, returns the number of attributes copied
	// numData is number of elements available to be filled in
	virtual uint32 GetOnlineAttributes(SCryLobbyUserData *pData, uint32 numData) = 0;

	// get the version number of the online attributes
	virtual int GetOnlineAttributesVersion() const = 0;

	// get the index of an online attribute via it's name
	virtual int GetOnlineAttributeIndexByName(const char *name) = 0;

	// fills the passed in array with the format of the online attributes
	virtual void GetOnlineAttributesDataFormat(SCryLobbyUserData *pData, uint32 numData) = 0;

	// return the number of online attributes
	virtual uint32 GetOnlineAttributeCount() = 0;

	// apply checksums to online data
	virtual void ApplyChecksums(SCryLobbyUserData* pData, uint32 numData) = 0;

	// check checksums are valid
	virtual bool ValidChecksums(const SCryLobbyUserData* pData, uint32 numData) = 0;
};

UNIQUE_IFACE struct ILevelRotationFile
{
	virtual ~ILevelRotationFile(){}
  virtual bool Save(XmlNodeRef r) = 0;
  virtual XmlNodeRef Load() = 0;
  virtual void Complete() = 0;
};

UNIQUE_IFACE struct IPlayerProfile
{
	virtual ~IPlayerProfile(){}
	// reset the profile
	virtual bool Reset() = 0; 

	// is this the default profile? it cannot be modified
	virtual bool IsDefault() const = 0;

	// override values with console player profile defaults
	virtual void LoadGamerProfileDefaults() = 0;

	// name of the profile
	virtual const char* GetName() = 0;

	// Id of the profile user
	virtual const char* GetUserId() = 0;

	// retrieve an action map 
	virtual IActionMap* GetActionMap(const char* name) = 0;

	// set the value of an attribute
	virtual bool SetAttribute(const char* name, const TFlowInputData& value) = 0;

	// re-set attribute to default value (basically removes it from this profile)
	virtual bool ResetAttribute(const char* name) = 0;

	// get the value of an attribute. if not specified optionally lookup in default profile
	virtual bool GetAttribute(const char* name, TFlowInputData& val, bool bUseDefaultFallback = true) const = 0;

	template<typename T> bool GetAttribute(const char* name, T& outVal, bool bUseDefaultFallback = true) const
	{
		TFlowInputData val;
		if (GetAttribute(name, val, bUseDefaultFallback) == false)
			return false;
		return val.GetValueWithConversion(outVal);
	}

	template<typename T> bool SetAttribute(const char* name, const T& val)
	{
		TFlowInputData data (val);
		return SetAttribute(name, data);
	}

	// get name all attributes available 
	// all in this profile and inherited from default profile
	virtual IAttributeEnumeratorPtr CreateAttributeEnumerator() = 0;

	// save game stuff
	virtual ISaveGameEnumeratorPtr CreateSaveGameEnumerator() = 0;
	virtual ISaveGame* CreateSaveGame() = 0;
	virtual ILoadGame* CreateLoadGame() = 0;
	virtual bool DeleteSaveGame(const char* name) = 0;

  virtual ILevelRotationFile* GetLevelRotationFile(const char* name) = 0;
};

#endif
