#ifndef _CSMAP_DATAFILE
#define _CSMAP_DATAFILE

#include <list>
#include <vector>
#include <map>
#include <set>
#include <fx.h>

// ==================
// === datakey class

class datakey
{
public:
	datakey() {}
	~datakey() {}

	//const FXString& key() const { return m_key; }
	const FXString& value() const { return m_value; }
	FXint type() const { return (m_type & TYPE_MASK); }
	const FXString key() const;

	void key(const FXString& s);
	void value(const FXString& s);

	bool isInt() const { return (m_type & TYPE_INTEGER) != 0; }
	FXint getInt() const;

	enum
	{
		TYPE_UNKNOWN,
		TYPE_EMPTY,
		TYPE_NAME,
		TYPE_DESCRIPTION,
		TYPE_TERRAIN,
		TYPE_ISLAND,
		TYPE_ID,
		TYPE_FACTION,
		TYPE_FACTIONNAME,
		TYPE_NUMBER,
		TYPE_TYPE,
		TYPE_SKILL,
		TYPE_KONFIGURATION,
		TYPE_VISIBILITY,
		TYPE_TURN,
		TYPE_SILVER,
		TYPE_RECRUITMENTCOST,
		TYPE_AURA,
		TYPE_AURAMAX,
		TYPE_OPTIONS,
		TYPE_EMAIL,
		TYPE_BANNER,
		TYPE_ORDERS_CONFIRMED,		// ejcOrdersConfirmed, special tag
		TYPE_CHARSET,
		TYPE_LAST,

		TYPE_MASK = (1 << 7) - 1,
		TYPE_INTEGER = 1 << 7
	};

	// parses str and create datakey object
	static FXint parseType(const FXString& type);
	FXbool parse(FXchar* str);

	typedef std::vector<datakey/*, boost::pool_allocator<datakey>*/ > list_type;
	typedef list_type::iterator itor;

	void iso2utf();			// converts strings from file-charset (iso8859-15) to utf8
	void utf2iso();

protected:
	FXint m_type;
	FXString m_key, m_value;
};
	
// =========================
// === attachment base class

// base class for block attachment classes
// these classes could contain additional information
// like new commands of a unit, region symbol flags (map) or
// pointer to previous' turns unit.
class attachment
{
public:
	attachment() {}
	virtual ~attachment() {}
};

class att_region : public attachment
{
public:
	typedef std::vector<float> peoplelist_t;

	peoplelist_t	people;

	FXString		island;			// name of island

	enum	// not yet used!!!
	{
		FLAG_STREET = (1 << 0),	// street in region (first)
		FLAG_STREET_NW= (1 << 0),	// street to north west
		FLAG_STREET_NO= (1 << 1),	// street to north east
		FLAG_STREET_O = (1 << 2),	// street to east
		FLAG_STREET_SO= (1 << 3),	// street to south east
		FLAG_STREET_SW= (1 << 4),	// street to south west
		FLAG_STREET_W = (1 << 5),	// street to west

		FLAG_STREET_UNDONE = (1 << 6),	// incomplete street in region (first)
		FLAG_STREET_UNDONE_NW= (1 << 6),	// incomplete street to north west
		FLAG_STREET_UNDONE_NO= (1 << 7),	// incomplete street to north east
		FLAG_STREET_UNDONE_O = (1 << 8),	// incomplete street to east
		FLAG_STREET_UNDONE_SO= (1 << 9),	// incomplete street to south east
		FLAG_STREET_UNDONE_SW= (1 << 10),	// incomplete street to south west
		FLAG_STREET_UNDONE_W = (1 << 11),	// incomplete street to west
	};
	int				streets;		// street symbols

	enum	// not yet used!!!
	{
		FLAG_CASTLE	   = (1 << 0),	// there is a building/tower/castle in this region
		FLAG_SHIP	   = (1 << 2),	// there are ships in this region
		FLAG_SHIPTRAVEL= (1 << 3),	// ships travelled the region
		
		FLAG_LIGHTHOUSE= (1 << 4),	// region is seen by lighthouse
		FLAG_TRAVEL    = (1 << 5),	// region is seen by traveling throu

		FLAG_MONSTER   = (1 << 6),	// monster in region
		FLAG_SEASNAKE  = (1 << 7),	// sea snake in region
		FLAG_DRAGON    = (1 << 8),	// dragon/wyrm in region

		FLAG_WORMHOLE  = (1 << 9),	// wurmloch in region

		FLAG_TROOPS	    = (1 << 10),	// units are in this region
		FLAG_REGION_SEEN= (1 << 11),	// region is seen by own units

		FLAG_GUARDED      = (1 << 12),		// unit guard the region
		FLAG_GUARDED_OWN  = (1 << 12),		// unit guard the region
		FLAG_GUARDED_ALLY = (1 << 13),		// unit guard the region
		FLAG_GUARDED_ENEMY= (1 << 14),		// unit guard the region
	};
	int				symbols;		// other symbols
};

class att_commands : public attachment
{
public:
	att_commands() : confirmed(false) { }

	typedef std::vector<FXString> cmdlist_t;

	FXString	header;
	cmdlist_t	prefix_lines;
	cmdlist_t	commands;
	cmdlist_t	postfix_lines;
	bool		confirmed;
};

// ====================
// === datablock class

// contains information for "NAME Info"
// like: MESSAGES 36535
// string() is empty for all type()s, except for TYPE_UNKNOWN!
class datablock
{
public:
	datablock();
	~datablock();

	FXint type() const { return m_type; }
	FXint info() const { return m_info; }	// identifier for UNIT, SCHIFF... plane for REGION
	FXString id() const;					// identifier as base36
	FXint x() const { return m_x; }
	FXint y() const { return m_y; }
	FXint terrain() const { return m_terrain; }
	FXint flags() const { return m_flags; }
	FXint depth() const { return m_depth; }
	const FXString string() const;
	datakey::list_type& data(){ return m_data; }

	void string(const FXString& s);
	void infostr(const FXString& s);
	void terrain(FXint terrain);
	void flags(FXint flags);				// set <flags> and unset all other flags
	void setFlags(FXint flags);				// set <flags> and don't modify the other flags
	void depth(FXint depth);				// set depth of the block

	::attachment* attachment() const { return m_attachment; }
	void attachment(::attachment* attach);
	
	const FXString terrainString() const;

	const FXString value(const FXchar* key) const;
	const FXString value(FXint key) const;
	FXint valueInt(const FXchar* key, FXint def = 0) const;
	FXint valueInt(FXint key, FXint def = 0) const;
	const datakey* valueKey(FXint key) const;

	// types of datablocks
	enum
	{
		TYPE_UNKNOWN,
		TYPE_VERSION,
		TYPE_OPTIONS,
		TYPE_FACTION,
		TYPE_GROUP,
		TYPE_ALLIANCE,
		TYPE_REGION,
		TYPE_ISLAND,
		TYPE_SCHEMEN,
		TYPE_RESOURCE,
		TYPE_PRICES,
		TYPE_DURCHREISE,
		TYPE_DURCHSCHIFFUNG,
		TYPE_BORDER,
		TYPE_SHIP,
		TYPE_BUILDING,
		TYPE_UNIT,
		TYPE_UNITMESSAGES,
		TYPE_TALENTS,
		TYPE_SPELLS,
		TYPE_COMBATSPELL,
		TYPE_ZAUBER,
		TYPE_KOMPONENTEN,
		TYPE_TRANK,
		TYPE_ZUTATEN,
		TYPE_ITEMS,
		TYPE_COMMANDS,
		TYPE_EFFECTS,
		TYPE_MESSAGE,
		TYPE_BATTLE,
		TYPE_MESSAGETYPE,
		TYPE_TRANSLATION,
		TYPE_LAST
	};

	// Terrain types
	enum
	{
		TERRAIN_UNKNOWN,
		TERRAIN_OCEAN,
		TERRAIN_SWAMP,
		TERRAIN_PLAINS,
		TERRAIN_DESERT,
		TERRAIN_FOREST,
		TERRAIN_HIGHLAND,
		TERRAIN_MOUNTAIN,
		TERRAIN_GLACIER,
		TERRAIN_VOLCANO,
		TERRAIN_VOLCANO_ACTIVE,
		TERRAIN_PACKICE,
		TERRAIN_ICEBERG,
        TERRAIN_ICEFLOE,
        TERRAIN_FLOOR,
        TERRAIN_WALL,
		TERRAIN_FIREWALL, TERRAIN_LASTPUBLIC = TERRAIN_FIREWALL,
		TERRAIN_MAHLSTROM,
        TERRAIN_LAST
	};

	// Flags for map icons
	enum
	{
		FLAG_CASTLE	= (1 << 0),		// there is a building/tower/castle in this region
		//FLAG_GUARDED= (1 << 1),		// unit guard the region
		FLAG_SHIP	= (1 << 2),		// there are ships in this region
		FLAG_SHIPTRAVEL= (1 << 3),	// ships travelled the region
		
		FLAG_LIGHTHOUSE= (1 << 4),	// region is seen by lighthouse
		FLAG_TRAVEL = (1 << 5),		// region is seen by traveling throu

		FLAG_MONSTER= (1 << 6),		// monster in region
		FLAG_SEASNAKE= (1 << 7),	// sea snake in region
		FLAG_DRAGON = (1 << 8),	// dragon/wyrm in region

		FLAG_WORMHOLE = (1 << 9),	// wurmloch in region

		FLAG_TROOPS	= (1 << 10),		// units are in this region
		
		FLAG_GUARD_OWN	= (1 << 11),	// region is guarded by *
		FLAG_GUARD_ALLY	= (1 << 12),
		FLAG_GUARD_ENEMY= (1 << 13),

		FLAG_REGION_OWN   = (1 << 14),	// region is owned by * (E3 only)
		FLAG_REGION_ALLY  = (1 << 15),
		FLAG_REGION_ENEMY = (1 << 16),

		FLAG_REGION_TAXES = (1 << 1),	// we got taxes in this region (E3 only)

		FLAG_STREET = (1 << 17),	// street in region (first)
		FLAG_STREET_NW= (1 << 17),	// street to north west
		FLAG_STREET_NO= (1 << 18),	// street to north east
		FLAG_STREET_O = (1 << 19),	// street to east
		FLAG_STREET_SO= (1 << 20),	// street to south east
		FLAG_STREET_SW= (1 << 21),	// street to south west
		FLAG_STREET_W = (1 << 22),	// street to west

		FLAG_STREET_UNDONE = (1 << 23),	// incomplete street in region (first)
		FLAG_STREET_UNDONE_NW= (1 << 23),	// incomplete street to north west
		FLAG_STREET_UNDONE_NO= (1 << 24),	// incomplete street to north east
		FLAG_STREET_UNDONE_O = (1 << 25),	// incomplete street to east
		FLAG_STREET_UNDONE_SO= (1 << 26),	// incomplete street to south east
		FLAG_STREET_UNDONE_SW= (1 << 27),	// incomplete street to south west
		FLAG_STREET_UNDONE_W = (1 << 28),	// incomplete street to west

		FLAG_REGION_SEEN = (1 << 31),	// region is seen by own units

		FLAG_BLOCKID_BIT0 = (1 << 29),	// number of ids: +1
		FLAG_BLOCKID_BIT1 = (1 << 30),	// +2

		FLAG_NONE	= 0				// no flags are set
	};

	// parses str and create datablock object
	static FXint parseType(const FXString& type);
	static FXint parseTerrain(const FXString& str);			// Plains, Mountains, ... Volcano
	static FXint parseSpecialTerrain(const FXString& str);	// Active volcano, ... (terrain that uses image of another terrain)
	static FXString planeName(FXint plane);					// Eressea,Astralraum,Weihnachtsinsel...
	FXbool parse(FXchar* str);

	typedef std::list<datablock/*, boost::fast_pool_allocator<datablock>*/ > list_type;
	typedef list_type::iterator itor;

	void iso2utf();			// converts strings from file-charset (iso8859-15) to utf8
	void utf2iso();

protected:
	FXint m_type, m_info;
	FXint m_x, m_y, m_terrain;
	FXint m_flags, m_depth;
	FXString m_string;
	datakey::list_type m_data;

	::attachment* m_attachment;

	struct blocknames
	{
		FXint id;
		const FXchar *name;
	};
	static blocknames BLOCKNAMES[];
};

class datafile
{
public:
	datafile();

	const FXString& filename() const { return m_filename; }
	void filename(const FXString& s) { m_filename = s; }
	
	const FXString& cmdfilename() const { return m_cmdfilename; }
	void cmdfilename(const FXString& s) { m_cmdfilename = s; }

	bool utf8cr() const { return m_utf8cr; }
	void utf8cr(bool utf8) { m_utf8cr = utf8; }

	FXString password() const { return m_cmds.password; }
	void password(const FXString& passwd) { m_cmds.password = passwd; }

	bool modifiedCmds() const { return m_cmds.modified; }
	void modifiedCmds(bool mod) { m_cmds.modified = mod; }

	int turn() const { return m_turn; }
	int recruitment() const { return m_recruitment; }
	datablock::itor activefaction() { return m_activefaction; }

	FXint load(const FXchar* filename);
	FXint save(const FXchar* filename, bool replace, bool merge_commands = false);
	void createHierarchy();
	void createHashTables();

	FXint loadCmds(const FXchar* filename);
	FXint saveCmds(const FXchar* filename, bool stripped, bool replace);

	datablock::list_type& blocks() { return m_blocks; }

	datablock::itor region(FXint x, FXint y, FXint plane);
	datablock::itor unit(FXint id);
	datablock::itor faction(FXint id);
	datablock::itor building(FXint id);
	datablock::itor ship(FXint id);
	datablock::itor island(FXint id);
	datablock::itor end();

	datablock::itor dummyToItor(const datablock* block);

	typedef std::list<datafile>::iterator itor;

	// data of selection state (what region, what unit is actually selected?)
	struct SelectionState
	{
        FXint selected;			// flags; what iterator contains valid information?
		FXint map;				// flags that indicate map states

		datablock::itor region, faction, building, ship, unit;

		FXint sel_x, sel_y, sel_plane;

		std::set<datablock*> regionsSelected;

		datablock::itor activefaction;

        FXint selChange, fileChange;	// incremented on changes to any datafile (loaded, closed or changed)

		// selected flags
		enum
		{
            REGION = (1<<0),			// region marked, iterator region is valid
			UNKNOWN_REGION = (1<<1),	// unknown region selected, sel_(x|y|plane) is valid
            MULTIPLE_REGIONS = (1<<2),	// some regions selected
			FACTION = (1<<3),
			BUILDING = (1<<4),
			SHIP = (1<<5),
			UNIT = (1<<6),
		};

		// map flags
		enum
		{
			ACTIVEFACTION = (1<<0),
			MAPCHANGED = (1<<1),
			NEWFILE = (1<<2)
		};

		// mini-c'tor
		SelectionState() : selected(0),map(0), selChange(0),fileChange(0) {}
	};

protected:
	// file names of report and command files
    FXString m_filename, m_cmdfilename;

	bool m_utf8cr;

	struct koordinates
	{
		koordinates(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}

		bool operator==(const koordinates& rhs) const
		{
            return x == rhs.x && y == rhs.y && z == rhs.z;
		}

		bool operator<(const koordinates& rhs) const
		{
			if (x < rhs.x) return true;
			if (x == rhs.x)
			{
				if (y < rhs.y) return true;
				if (y == rhs.y)
					if (z < rhs.z)
						return true;
			}
			return false;
		}

		int x, y, z;
	};

	// command file stuff
	struct cmds_t
	{
		bool modified;
		FXString password;

        // command lines before any units
		std::vector<FXString> prefix_lines;

		// command lines in REGIONs
		typedef std::map<koordinates, att_commands> region_map_t;
		typedef std::list< std::pair<koordinates, std::vector<int> > > region_list_t;

		region_map_t region_lines;
		region_list_t region_order;		// keeps the correct order of regions and units in it

	} m_cmds;

	// the data blocks
	datablock::list_type m_blocks;

	// some collected information
	int m_recruitment;
	int m_turn;
	datablock::itor m_activefaction;

	// hash tables
	std::map<FXint, datablock::itor> m_units, m_factions, m_buildings, m_ships, m_islands;
	std::map<koordinates, datablock::itor> m_regions;

	// temporaries (like FACTION block that don't exist in CR)
	datablock::list_type m_dummyBlocks;
};

#endif //_CSMAP_DATAFILE
