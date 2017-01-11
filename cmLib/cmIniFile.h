
#ifndef CM_INIFILE_H
#define CM_INIFILE_H

namespace cm
{
struct ci_less_a
{
  bool operator() (const std::string & s1, const std::string & s2) const
  {
	return _stricmp(s1.c_str(), s2.c_str()) < 0;
  }
};

class cmIniFileA
{
	public:
		cmIniFileA();
		~cmIniFileA();

        // Used to save the data back to the file or your choice
		bool Save( const std::string& fileName, bool bOrder = false);

		// Save data to an output stream
		void Save( std::ostream& output );

		//Save data to an output stream according to original order
		void SaveByOrder(std::ostream& output);

		// Loads the Reads the data in the ini file into the IniFile object
		bool Load( const std::string& fileName , bool bMerge = false );

		// Load data from an input stream
		void Load( std::istream& input , bool bMerge = false );

    public:
            class cmIniMergeA
            {
                public:
                    explicit cmIniMergeA(cmIniFileA& ini):_ini(ini){}
                    std::istream &operator()(std::istream& input) const
                    {
                        _ini.Load( input , true );
                        return input;
                    }
                private:
                cmIniFileA& _ini;
            };
	public:
            class cmIniSectionA
			{
				friend class cmIniFileA; // Allow cmIniFileA to create sections
				public:
					class cmIniKeyA
					{
						friend class cmIniSectionA; // Allow cmIniSectionA to create keys
						private: // cmIniFileA acts as a class factory for cmIniSectionA Objects
							cmIniKeyA( cmIniSectionA* pSection , std::string sKeyName );
							cmIniKeyA( const cmIniKeyA& ); // No Copy
							cmIniKeyA& operator=(const cmIniKeyA&); // No Copy
							~cmIniKeyA( );
						public:
							// Sets the value of the key
							void SetValue( std::string sValue );
							// Returns the value of the Key
							std::string GetValue() const;
							// Sets the key name, returns true on success, fails if the section
							// name sKeyName already exists
							bool SetKeyName( std::string sKeyName );
							// Returns the name of the Key
							std::string GetKeyName() const;
							UINT m_uiIndex;//original order of key
						private:
							// Name of the Key
							std::string m_sKeyName;
							// Value associated
							std::string m_sValue;
							// Pointer to the parent cmIniSectionA
							cmIniSectionA* m_pSection;
					}; // End of cmIniKeyA
					typedef std::map<std::string,cmIniKeyA*,ci_less_a> KeyMapA;
					typedef std::map<UINT,cmIniKeyA*> KeyOrderMapA;
					#ifdef _WIN32
                        // Added for VC6 Support
                        #if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
                            friend class cmIniKeyA;
                        #endif
                    #endif
				private: // cmIniSectionA acts as a class factory for cmIniKeyA Objects
					cmIniSectionA( cmIniFileA* pIniFile , std::string sSectionName );
					cmIniSectionA( const cmIniSectionA& ); // No Copy
					cmIniSectionA& operator=(const cmIniSectionA&); // No Copy
					~cmIniSectionA( );
				public:
					// Adds a key to the cmIniSectionA object, returns a cmIniKeyA pointer to the new or existing object
					cmIniKeyA* AddKey( std::string sKeyName );
					// Removes a single key by pointer
					void RemoveKey( cmIniKeyA* pKey );
					// Removes a single key by string
					void RemoveKey( std::string sKey );
                    // Removes all the keys in the section
					void RemoveAllKeys( );
					// Returns a cmIniKeyA pointer to the key by name, NULL if it was not found
					cmIniKeyA* GetKey( std::string sKeyName ) const;
					// Returns all keys in the section by KeyList ref
					const KeyMapA& GetKeys() const;
					// Returns a KeyValue at a certain section
					std::string GetKeyValue( std::string sKey ) const;
					// Sets a KeyValuePair at a certain section
					void SetKeyValue( std::string sKey, std::string sValue );
					// Sets the section name, returns true on success, fails if the section
					// name sSectionName already exists
					bool SetSectionName( std::string sSectionName );
					// Returns the section name
					std::string GetSectionName() const;
					UINT m_uiIndex;//original order of section
				private:
					// cmIniFileA pointer back to the object that instanciated the section
					cmIniFileA* m_pIniFile;
					// Name of the section
					std::string m_sSectionName;
					// List of cmIniKeyA pointers ( Keys in the section )
					KeyMapA m_keys;
					UINT m_uiNextKeyIndex; //used to save original order for key
					
			}; // End of cmIniSectionA
		// Typedef of a List of cmIniSectionA pointers
		typedef std::map<std::string,cmIniSectionA*,ci_less_a> SecMapA;
		typedef std::map<UINT,cmIniSectionA*> SecOrderMapA;
        #ifdef _WIN32
            // Added for VC6 Support
            #if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
                friend class cmIniSectionA;
            #endif
        #endif
	public:
		// Adds a section to the cmIniFileA object, returns a cmIniFileA pointer to the new or existing object
		cmIniSectionA* AddSection( std::string sSection );
		// Removes section by pointer
		void RemoveSection( cmIniSectionA* pSection );
		// Removes a section by its name sSection
		void RemoveSection( std::string sSection );
		// Removes all existing sections
		void RemoveAllSections( );
		// Returns a cmIniSectionA* to the section by name, NULL if it was not found
		cmIniSectionA* GetSection( std::string sSection ) const;
		// Gets all the section in the ini file
		const SecMapA& GetSections() const;
		// Returns a KeyValue at a certain section
		std::string GetKeyValue( std::string sSection, std::string sKey ) const;
		// Sets a KeyValuePair at a certain section
		void SetKeyValue( std::string sSection, std::string sKey, std::string sValue );
		// Renames an existing section returns true on success, false if the section didn't exist or there was another section with the same sNewSectionName
		bool RenameSection( std::string sSectionName  , std::string sNewSectionName );
		// Renames an existing key returns true on success, false if the key didn't exist or there was another section with the same sNewSectionName
		bool RenameKey( std::string sSectionName  , std::string sKeyName , std::string sNewKeyName);
	private:
		cmIniFileA( const cmIniFileA&); // No Copy
		cmIniFileA& operator=(const cmIniFileA&); // No Copy
		// List of cmIniSectionA pointers ( List of sections in the class )
		SecMapA m_sections;
		UINT m_uiNextSecIndex;//used to save original order for section
}; // End of cmIniFileA

// Basic typedefs for ease of use
typedef cmIniFileA::cmIniMergeA cmIniMergeA;
typedef cmIniFileA::cmIniSectionA cmIniSectionA;
typedef cmIniSectionA::cmIniKeyA cmIniKeyA;


// Map Types
typedef cmIniSectionA::KeyMapA KeyMapA;
typedef cmIniSectionA::KeyOrderMapA KeyOrderMapA;
typedef cmIniFileA::SecMapA SecMapA;
typedef cmIniFileA::SecOrderMapA SecOrderMapA;


std::ostream& operator<<(std::ostream& output, cmIniFileA& obj);
std::istream& operator>>(std::istream& input, cmIniFileA& obj);
std::istream& operator>>(std::istream& input, cmIniMergeA merger);

// Unicode Class Definition


struct ci_less_w
{
  bool operator() (const std::wstring & s1, const std::wstring & s2) const
  {
	return _wcsicmp(s1.c_str(), s2.c_str()) < 0;
  }
};


class cmIniFileW
{
	public:
		cmIniFileW();
		~cmIniFileW();

        // Used to save the data back to the file or your choice
		bool Save( const std::wstring& fileName,bool bOrder = false );

        // Save data to an out stream
		void Save( std::wostream& output );

		//Save data to an output stream according to original order
		void SaveByOrder(std::wostream& output);

		// Loads the Reads the data in the ini file into the IniFile object
		bool Load( const std::wstring& fileName , bool bMerge = false );

		// Load data from an input stream
		void Load( std::wistream& input , bool bMerge = false );
    public:
            class cmIniMergeW
            {
                public:
                    explicit cmIniMergeW(cmIniFileW& ini):_ini(ini){}
                    std::wistream& operator()(std::wistream& input) const
                    {
                    _ini.Load( input , true );
                    return input;
                    }
                private:
                    cmIniFileW& _ini;
            };
	public:
			class cmIniSectionW
			{
				friend class cmIniFileW; // Allow cmIniFileW to create sections
				public:
					class cmIniKeyW
					{
						friend class cmIniSectionW; // Allow cmIniSectionW to create keys
						private: // cmIniFileW acts as a class factory for cmIniSectionW Objects
							cmIniKeyW( cmIniSectionW* pSection , std::wstring sKeyName );
							cmIniKeyW( const cmIniKeyW& ); // No Copy
							cmIniKeyW& operator=(const cmIniKeyW&); // No Copy
							~cmIniKeyW( );
						public:
							// Sets the value of the key
							void SetValue( std::wstring sValue );
							// Returns the value of the Key
							std::wstring GetValue() const;
							// Sets the key name, returns true on success, fails if the section
							// name sKeyName already exists
							bool SetKeyName( std::wstring sKeyName );
							// Returns the name of the Key
							std::wstring GetKeyName() const;
							UINT m_uiIndex;//original order of key
						private:
							// Name of the Key
							std::wstring m_sKeyName;
							// Value associated
							std::wstring m_sValue;
							// Pointer to the parent cmIniSectionW
							cmIniSectionW* m_pSection;
					}; // End of cmIniKeyW
					typedef std::map<std::wstring,cmIniKeyW*,ci_less_w> KeyMapW;
					typedef std::map<UINT,cmIniKeyW*> KeyOrderMapW;
					#ifdef _WIN32
                        // Added for VC6 Support
                        #if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
                            friend class cmIniKeyW;
                        #endif
                    #endif
				private: // cmIniSectionW acts as a class factory for cmIniKeyW Objects
					cmIniSectionW( cmIniFileW* pIniFile , std::wstring sSectionName );
					cmIniSectionW( const cmIniSectionW& ); // No Copy
					cmIniSectionW& operator=(const cmIniSectionW&); // No Copy
					~cmIniSectionW( );
				public:
					// Adds a key to the cmIniSectionW object, returns a cmIniKeyW pointer to the new or existing object
					cmIniKeyW* AddKey( std::wstring sKeyName );
					// Removes a single key by pointer
					void RemoveKey( cmIniKeyW* pKey );
					// Removes a single key by string
					void RemoveKey( std::wstring sKey );
                    // Removes all the keys in the section
					void RemoveAllKeys( );
					// Returns a cmIniKeyW pointer to the key by name, NULL if it was not found
					cmIniKeyW* GetKey( std::wstring sKeyName ) const;
					// Returns all keys in the section by KeyList ref
					const KeyMapW& GetKeys() const;
					// Returns a KeyValue at a certain section
					std::wstring GetKeyValue( std::wstring sKey ) const;
					// Sets a KeyValuePair at a certain section
					void SetKeyValue( std::wstring sKey, std::wstring sValue );
					// Sets the section name, returns true on success, fails if the section
					// name sSectionName already exists
					bool SetSectionName( std::wstring sSectionName );
					// Returns the section name
					std::wstring GetSectionName() const;
					UINT m_uiIndex;//original order of section
				private:
					// cmIniFileW pointer back to the object that instanciated the section
					cmIniFileW* m_pIniFile;
					// Name of the section
					std::wstring m_sSectionName;
					// List of cmIniKeyW pointers ( Keys in the section )
					KeyMapW m_keys;
					UINT m_uiNextKeyIndex;//used to save original order for key
			}; // End of cmIniSectionW
		// Typedef of a List of cmIniSectionW pointers
		typedef std::map<std::wstring,cmIniSectionW*,ci_less_w> SecMapW;
		typedef std::map<UINT,cmIniSectionW*> SecOrderMapW;
        #ifdef _WIN32
            // Added for VC6 Support
            #if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
                friend class cmIniSectionW;
            #endif
        #endif
	public:
		// Adds a section to the cmIniFileW object, returns a cmIniFileW pointer to the new or existing object
		cmIniSectionW* AddSection( std::wstring sSection );
		// Removes section by pointer
		void RemoveSection( cmIniSectionW* pSection );
		// Removes a section by its name sSection
		void RemoveSection( std::wstring sSection );
		// Removes all existing sections
		void RemoveAllSections( );
		// Returns a cmIniSectionW* to the section by name, NULL if it was not found
		cmIniSectionW* GetSection( std::wstring sSection ) const;
		// Gets all the section in the ini file
		const SecMapW& GetSections() const;
		// Returns a KeyValue at a certain section
		std::wstring GetKeyValue( std::wstring sSection, std::wstring sKey ) const;
		// Sets a KeyValuePair at a certain section
		void SetKeyValue( std::wstring sSection, std::wstring sKey, std::wstring sValue );
		// Renames an existing section returns true on success, false if the section didn't exist or there was another section with the same sNewSectionName
		bool RenameSection( std::wstring sSectionName  , std::wstring sNewSectionName );
		// Renames an existing key returns true on success, false if the key didn't exist or there was another section with the same sNewSectionName
		bool RenameKey( std::wstring sSectionName  , std::wstring sKeyName , std::wstring sNewKeyName);
	private:
		cmIniFileW( const cmIniFileW&); // No Copy
		cmIniFileW& operator=(const cmIniFileW&); // No Copy
		// List of cmIniSectionW pointers ( List of sections in the class )
		SecMapW m_sections;
		UINT m_uiNextSecIndex;//used to save original order for section
}; // End of cmIniFileW

// Basic typedefs for ease of use
typedef cmIniFileW::cmIniMergeW cmIniMergeW;
typedef cmIniFileW::cmIniSectionW cmIniSectionW;
typedef cmIniSectionW::cmIniKeyW cmIniKeyW;


// Map Types
typedef cmIniSectionW::KeyMapW KeyMapW;
typedef cmIniSectionW::KeyOrderMapW KeyOrderMapW;
typedef cmIniFileW::SecMapW SecMapW;
typedef cmIniFileW::SecOrderMapW SecOrderMapW;

std::wostream& operator<<(std::wostream& output, cmIniFileW& obj);
std::wistream& operator>>(std::wistream& input, cmIniFileW& obj);
std::wistream& operator>>(std::wistream& input, cmIniMergeW merger);

// Additional defines
#ifdef _UNICODE
	typedef cmIniMergeW cmIniMerge;
    typedef cmIniFileW cmIniFile;
    typedef cmIniSectionW cmIniSection;
    typedef cmIniKeyW cmIniKey;
    typedef KeyMapW KeyMap;
    typedef SecMapW SecMap;
	typedef KeyOrderMapW KeyOrderMap;
    typedef SecOrderMapW SecOderMap;
#else

	typedef cmIniMergeA cmIniMerge;
    typedef cmIniFileA cmIniFile;
    typedef cmIniSectionA cmIniSection;
    typedef cmIniKeyA cmIniKey;
    typedef KeyMapA KeyMap;
    typedef SecMapA SecMap;
	typedef KeyOrderMapA KeyOrderMap;
    typedef SecOrderMapA SecOderMap;
#endif
}
#endif

