#include <sstream>
#include <vector>

#include "regionlist.h"
#include "symbols.h"

#include "FXMenuSeparatorEx.h"

// *********************************************************************************************************
// *** FXRegionItem implementation

class FXAPI FXRegionItem : public FXTreeItem
{
	FXDECLARE(FXRegionItem)
	friend class FXTreeList;
	friend class FXDirList;
protected:
	static FXFont *boldfont;

	bool bold;

	FXRegionItem() : FXTreeItem(), bold() {}

	virtual void draw(const FXTreeList* list,FXDC& dc,FXint x,FXint y,FXint w,FXint h) const;
	virtual FXint hitItem(const FXTreeList* list,FXint xx,FXint yy) const;
	virtual FXint getWidth(const FXTreeList* list) const;
	virtual FXint getHeight(const FXTreeList* list) const;

public:
	/// Constructor
	FXRegionItem(const FXString& text,FXIcon* oi=NULL,FXIcon* ci=NULL,void* ptr=NULL) : FXTreeItem(text, oi, ci, ptr), bold() {}

	bool isBold() const
	{
		return bold;
	}

	void setBold(bool b = true)
	{
		bold = b;
	}

	// ----------------------------------
	static void setBoldFont(FXFont* font)
	{
		boldfont = font;
	}
};

/*static */ FXFont* FXRegionItem::boldfont;

static const int ICON_SPACING = 4;	// Spacing between parent and child in x direction
static const int TEXT_SPACING = 4;	// Spacing between icon and text
static const int SIDE_SPACING = 4;	// Spacing between side and item

// Object implementation
FXIMPLEMENT(FXRegionItem,FXTreeItem,NULL,0)

// Draw item
void FXRegionItem::draw(const FXTreeList* list,FXDC& dc,FXint xx,FXint yy,FXint,FXint hh) const
{
  register FXIcon *icon=(state&OPENED)?openIcon:closedIcon;
  register FXFont *font=list->getFont();
  if (isBold() && boldfont)
		dc.setFont(font = boldfont);
  register FXint th=0,tw=0,ih=0,iw=0;
  xx+=SIDE_SPACING/2;
  if(icon){
    iw=icon->getWidth();
    ih=icon->getHeight();
    dc.drawIcon(icon,xx,yy+(hh-ih)/2);
    xx+=ICON_SPACING+iw;
    }
  if(!label.empty()){
    tw=4+font->getTextWidth(label.text(),label.length());
    th=4+font->getFontHeight();
    yy+=(hh-th)/2;
    if(isSelected()){
      dc.setForeground(list->getSelBackColor());
      dc.fillRectangle(xx,yy,tw,th);
      }
    if(hasFocus()){
      dc.drawFocusRectangle(xx+1,yy+1,tw-2,th-2);
      }
    if(!isEnabled())
      dc.setForeground(makeShadowColor(list->getBackColor()));
    else if(isSelected())
      dc.setForeground(list->getSelTextColor());
    else
      dc.setForeground(list->getTextColor());
    dc.drawText(xx+2,yy+font->getFontAscent()+2,label);
    }

	if (isBold())
		dc.setFont(list->getFont());
}

// See if item got hit, and where:- 1 is icon, 2 is text
FXint FXRegionItem::hitItem(const FXTreeList* list,FXint xx,FXint yy) const
{
  register FXint oiw=0,ciw=0,oih=0,cih=0,tw=0,th=0,iw,ih,ix,iy,tx,ty,h;
  register FXFont *font=list->getFont();
  if (isBold() && boldfont)
		font = boldfont;

  if(openIcon){
    oiw=openIcon->getWidth();
    oih=openIcon->getHeight();
    }
  if(closedIcon){
    ciw=closedIcon->getWidth();
    cih=closedIcon->getHeight();
    }
  if(!label.empty()){
    tw=4+font->getTextWidth(label.text(),label.length());
    th=4+font->getFontHeight();
    }
  iw=FXMAX(oiw,ciw);
  ih=FXMAX(oih,cih);
  h=FXMAX(th,ih);
  ix=SIDE_SPACING/2;
  tx=SIDE_SPACING/2;
  if(iw) tx+=iw+ICON_SPACING;
  iy=(h-ih)/2;
  ty=(h-th)/2;

  // In icon?
  if(ix<=xx && iy<=yy && xx<ix+iw && yy<iy+ih) return 1;

  // In text?
  if(tx<=xx && ty<=yy && xx<tx+tw && yy<ty+th) return 2;

  // Outside
  return 0;
  }

// Get item width
FXint FXRegionItem::getWidth(const FXTreeList* list) const
{
  register FXint w=0,oiw=0,ciw=0;
  register FXFont *font=list->getFont();
  if (isBold() && boldfont)
		font = boldfont;

  if(openIcon) oiw=openIcon->getWidth();
  if(closedIcon) ciw=closedIcon->getWidth();
  w=FXMAX(oiw,ciw);
  if(!label.empty()){
    if(w) w+=ICON_SPACING;
    w+=4+font->getTextWidth(label.text(),label.length());
    }
  return SIDE_SPACING+w;
  }

// Get item height
FXint FXRegionItem::getHeight(const FXTreeList* list) const
{
  register FXint th=0,oih=0,cih=0;
  register FXFont *font=list->getFont();
  if (isBold() && boldfont)
		font = boldfont;

  if(openIcon) oih=openIcon->getHeight();
  if(closedIcon) cih=closedIcon->getHeight();
  if(!label.empty()) th=4+font->getFontHeight();
  return FXMAX3(th,oih,cih);
  }

// *********************************************************************************************************
// *** FXRegionList implementation

FXDEFMAP(FXRegionList) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_CLICKED,			0,									FXRegionList::onSelected), 

	FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	0,								FXRegionList::onPopup), 
	FXMAPFUNC(SEL_COMMAND,			FXRegionList::ID_POPUP_CLICKED,		FXRegionList::onPopupClicked), 

	FXMAPFUNC(SEL_COMMAND,			FXRegionList::ID_TOGGLEOWNFACTIONGROUP, FXRegionList::onToggleOwnFactionGroup), 
	FXMAPFUNC(SEL_UPDATE,			FXRegionList::ID_TOGGLEOWNFACTIONGROUP, FXRegionList::onUpdateOwnFactionGroup),

	FXMAPFUNC(SEL_COMMAND,			FXRegionList::ID_UPDATE,			FXRegionList::onMapChange), 
	FXMAPFUNC(SEL_QUERY_HELP,		0,									FXRegionList::onQueryHelp), 
}; 

FXIMPLEMENT(FXRegionList,FXTreeList,MessageMap, ARRAYNUMBER(MessageMap))

FXRegionList::FXRegionList(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXTreeList(p, tgt,sel, opts, x, y, w, h)
{
	// modify list style
	setListStyle(getListStyle()|TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES);

	// init variables
	files = NULL;

	active_faction_group = false;

	// create/load images for terrain types
	for (int i = 0; i < datablock::TERRAIN_LAST; i++)
		terrainIcons[i] = new FXGIFIcon(getApp(), data::terrainSymbols[i], 0, IMAGE_ALPHAGUESS);

	green = new FXGIFIcon(getApp(), data::green, 0, IMAGE_ALPHAGUESS);
	red = new FXGIFIcon(getApp(), data::red, 0, IMAGE_ALPHAGUESS);
	blue = new FXGIFIcon(getApp(), data::blue, 0, IMAGE_ALPHAGUESS);
	cyan = new FXGIFIcon(getApp(), data::cyan, 0, IMAGE_ALPHAGUESS);
	yellow = new FXGIFIcon(getApp(), data::yellow, 0, IMAGE_ALPHAGUESS);
	orange = new FXGIFIcon(getApp(), data::orange, 0, IMAGE_ALPHAGUESS);
	gray = new FXGIFIcon(getApp(), data::gray, 0, IMAGE_ALPHAGUESS);
	black = new FXGIFIcon(getApp(), data::black, 0, IMAGE_ALPHAGUESS);
}

void FXRegionList::create()
{
	FXTreeList::create();

	FXFontDesc fontdesc;
	getFont()->getFontDesc(fontdesc);
	fontdesc.weight = FXFont::Bold;
	boldfont = new FXFont(getApp(), fontdesc);
	boldfont->create();

	FXRegionItem::setBoldFont(boldfont);

	// create icons for region list
	for (int i = 0; i < datablock::TERRAIN_LAST; i++)
		terrainIcons[i]->create();

	green->create();
	red->create();
	blue->create();
	cyan->create();
	yellow->create();
	orange->create();
	gray->create();
	black->create();
}

FXRegionList::~FXRegionList()
{
	delete boldfont;

	for (int i = 0; i < datablock::TERRAIN_LAST; i++)
		delete terrainIcons[i];

	delete green;
	delete red;
	delete blue;
	delete cyan;
	delete yellow;
	delete orange;
	delete gray;
	delete black;
}

long FXRegionList::onToggleOwnFactionGroup(FXObject* sender, FXSelector, void* ptr)
{
	active_faction_group = !active_faction_group;

	// connected to a datafile list?
	if (!files)
		return 1;

	// map change notify rebuilds treelist
	datafile::SelectionState state = selection;
	if (files->empty())
		state.selected = 0,
		state.map = 0;

	state.map |= state.MAPCHANGED;
	getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;  
}

long FXRegionList::onUpdateOwnFactionGroup(FXObject* sender, FXSelector, void* ptr)
{
	sender->handle(this, FXSEL(SEL_COMMAND, active_faction_group?ID_CHECK:ID_UNCHECK), NULL);
	return 1;
}

void FXRegionList::mapfiles(std::list<datafile> *f)
{
    files = f;
}

long FXRegionList::onSelected(FXObject*,FXSelector,void*)
{
	// connected to a datafile list?
	if (!files)
		return 0;

	if (!getCurrentItem())	// kein Item markiert.
		return 0;

	datablock* data = (datablock*)getCurrentItem()->getData();
	datablock* parentdata = NULL;
	if (getCurrentItem()->getParent())
		parentdata = (datablock*)getCurrentItem()->getParent()->getData();

	datablock::itor main = files->begin()->blocks().end();
	datablock::itor parent = files->begin()->blocks().end();

	for (datafile::itor file = files->begin(); file != files->end(); file++)
		for (datablock::itor block = file->blocks().begin(); block != file->blocks().end(); block++)
		{
			// nothing to search for
			if (!data && !parentdata)
				break;

			// handle only regions, factions (+alliances), buildings, ships and units
			if (block->type() != datablock::TYPE_REGION &&
				block->type() != datablock::TYPE_ALLIANCE &&
				block->type() != datablock::TYPE_FACTION &&
				block->type() != datablock::TYPE_BUILDING &&
				block->type() != datablock::TYPE_SHIP &&
				block->type() != datablock::TYPE_UNIT)
				continue;

			if (data && data == &*block)
			{
				main = block;
				data = NULL;		// to indicate that block was found.
			}
			else if (parentdata && parentdata == &*block)
			{
				parent = block;
				parentdata = NULL;	// found
			}
		}

	if (main == files->begin()->blocks().end())
	{
        main = files->begin()->dummyToItor(data);
	}

	if (main == files->begin()->blocks().end())
	{
		main = parent;
		parent = files->begin()->blocks().end();
	}

	if (main != files->begin()->blocks().end())
	{
		// send new selection to main window
		datafile::SelectionState state;

		if (main->type() == datablock::TYPE_REGION)
		{
			state.selected = state.REGION;
			state.region = main;
		}
		if (main->type() == datablock::TYPE_FACTION)
		{
			state.selected = state.FACTION;
			state.faction = main;

			if (parent != files->begin()->blocks().end())
			{
				state.selected |= state.REGION;
				state.region = parent;
			}
		}
		if (main->type() == datablock::TYPE_BUILDING)
		{
			state.selected = state.BUILDING;
			state.building = main;

			if (parent != files->begin()->blocks().end())
			{
				state.selected |= state.FACTION;
				state.faction = parent;
			}
		}
		if (main->type() == datablock::TYPE_SHIP)
		{
			state.selected = state.SHIP;
			state.ship = main;

			if (parent != files->begin()->blocks().end())
			{
				state.selected |= state.FACTION;
				state.faction = parent;
			}
		}
		if (main->type() == datablock::TYPE_UNIT)
		{
			state.selected = state.UNIT;
			state.unit = main;

			if (parent != files->begin()->blocks().end())
			{
				state.selected |= state.FACTION;
				state.faction = parent;
			}			
		}

		getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
		return 1;
	}

	return 0;
}

long FXRegionList::onPopup(FXObject* sender,FXSelector sel, void* ptr)
{
	FXTreeList::onRightBtnRelease(sender, sel, ptr);

	FXEvent *event = (FXEvent*)ptr;

	// connected to a datafile list?
	if (!files)
		return 0;

	// dont't show popup if mouse has moved
	if (event->last_x != event->click_x || event->last_y != event->click_y)
		return 0;
	
	// create popup
	FXTreeItem *item = getItemAt(event->click_x, event->click_y);
	if (!item)
		return 0;

	FXMenuPane *menu = new FXMenuPane(this);

	std::vector<FXString> labels, texts;

	datablock* block = (datablock*)item->getData();
	if (block)
	{
		if (block->type() == datablock::TYPE_REGION)
		{
			FXString name = block->value(datakey::TYPE_NAME);
			FXString terrain = block->terrainString();

			if (name.length())
			{
				labels.push_back("Name: "+name);
				texts.push_back(name);
			}

			labels.push_back("Terrain: "+terrain);
			texts.push_back(terrain);
		}
		else if (block->type() == datablock::TYPE_UNIT)
		{
			FXString name = block->value(datakey::TYPE_NAME);
			FXString id = block->id();
			FXString number = block->value(datakey::TYPE_NUMBER);
			FXString type = block->value(datakey::TYPE_TYPE);

			if (name.length())
			{
				labels.push_back("Name: "+name);
				texts.push_back(name);
			}

			labels.push_back("Nummer: "+id);
			texts.push_back(id);

			labels.push_back("Anzahl: "+number);
			texts.push_back(number);

			labels.push_back("Rasse: "+type);
			texts.push_back(type);
		}
		else if (block->type() == datablock::TYPE_FACTION)
		{
			FXString name = block->value(datakey::TYPE_FACTIONNAME);
			FXString id = block->id();

			if (name.length())
			{
				labels.push_back("Name: "+name);
				texts.push_back(name);
			}

			if (id == "-1")
			{
				labels.push_back("anonym");
				texts.push_back("anonym");
			}
			else
			{
				labels.push_back("Nummer: "+id);
				texts.push_back(id);
			}

			FXString email = block->value(datakey::TYPE_EMAIL);
			FXString banner = block->value(datakey::TYPE_BANNER);

			if (email.length())
			{
				labels.push_back("eMail: "+email);
				texts.push_back(email);
			}

			if (banner.length())
			{
				labels.push_back("Banner: "+banner);
				texts.push_back(banner);
			}
		}
	}

	new FXMenuSeparatorEx(menu, item->getText());

	for (unsigned i = 0; i < labels.size() && i < texts.size(); i++)
	{
		FXMenuCommand *menuitem = new FXMenuCommand(menu, labels[i], NULL, this,ID_POPUP_CLICKED);
		menuitem->setUserData((void*)texts[i].text());
	}

	// show popup
	menu->create();
	menu->popup(NULL, event->root_x,event->root_y);

    getApp()->runModalWhileShown(menu);

	delete menu;
	return 1;
}

long FXRegionList::onPopupClicked(FXObject* sender,FXSelector, void*)
{
	if (sender && sender->isMemberOf(&FXMenuCommand::metaClass))
	{
		FXMenuCommand *cmd = (FXMenuCommand*)sender;

		// set clipboard to text stored in FXMenuCommands userdata
		getTarget()->handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), cmd->getUserData());
		return 1;
	}

	return 0;
}

// rekursivly searches item with userdata=data in treeitem list
FXTreeItem* FXRegionList::findTreeItem(FXTreeItem* first, void* data)
{
	FXTreeItem *item = first;
	while (item)
	{
		if (item->getData() == data)
			return item;

		if (item->getFirst())
		{
            FXTreeItem* found = findTreeItem(item->getFirst(), data);
			if (found)
				return found;
		}

		item = item->getNext();
	}

	return NULL;
}

long FXRegionList::onMapChange(FXObject* /*sender*/, FXSelector, void* ptr)
{
	datafile::SelectionState *state = (datafile::SelectionState*)ptr;

	// connected to a datafile list?
	if (!files)
		return 0;

	// any data changed, so need to update list?
	if (selection.fileChange != state->fileChange)
	{
		selection.fileChange = state->fileChange;
		selection.map = state->map;
		selection.activefaction = state->activefaction;

		// clear list and build a new one from data in this->files
		clearItems();

		FXString label, name, terrainString;

		for (datafile::itor file = files->begin(); file != files->end(); file++)
		{
			datablock::itor end = file->blocks().end();

			for (datablock::itor block = file->blocks().begin(); block != end; block++)
			{
				// handle only regions
				if (block->type() != datablock::TYPE_REGION)
					continue;

				terrainString = block->terrainString();

				name = block->value(datakey::TYPE_NAME);

				if (name.empty())
					name = terrainString;
				if (name.empty())
					name = "Unbekannt";

				if (block->info())
					label.format("%s (%d,%d,%s)", name.text(), block->x(), block->y(), datablock::planeName(block->info()).text());
				else
					label.format("%s (%d,%d)", name.text(), block->x(), block->y());

				// select terrain image
				FXint terrain = block->terrain();

				FXTreeItem* region = NULL;
				FXString regionlabel = label;

				std::map<FXint,FXTreeItem*> factions;

				datablock::itor end = file->blocks().end();
				datablock::itor unit = block;
				for (unit++; unit != end && unit->depth() > block->depth(); unit++)
				{
					// display units until next region
					if (unit->type() != datablock::TYPE_UNIT)
						continue;

					// get faction id, -1 means unknown faction (or stealth/anonymous)
					FXint factionId = unit->valueInt(datakey::TYPE_FACTION, -1);

					FXTreeItem* &entry = factions[factionId];
					if (!entry)
					{
						datablock::itor faction = file->faction(factionId);
						datablock* facPtr = NULL;
						if (faction != file->blocks().end())
							facPtr = &*faction;

						if (faction != file->blocks().end())
						{
							name = faction->value(datakey::TYPE_FACTIONNAME);
							if (faction->info() < 0)
							{
								if (name.empty())
									name = "Parteigetarnt";
								label.format("%s", name.text());
							}
							else
							{
								if (name.empty())
									name = "Unbekannt";
								label.format("%s (%s)", name.text(), faction->id().text());
							}
						}
						else
						{
							datablock block;
							block.infostr(FXStringVal(factionId));

							label.format("Unbekannt (%s)", block.id().text());
						}

						FXIcon* icon = red;
						if (factionId == 0)
							icon = black;
						else if (factionId == -1)
							icon = gray;

						if (selection.map & selection.ACTIVEFACTION)
						{
							if (faction == selection.activefaction)
								icon = blue;

							datablock::itor block = selection.activefaction;
							for (block++; block != files->begin()->end(); block++)
							{
								if (block->type() != datablock::TYPE_ALLIANCE &&
									block->type() != datablock::TYPE_ITEMS &&
									block->type() != datablock::TYPE_OPTIONS &&
									block->type() != datablock::TYPE_GROUP)
									break;

								if (block->type() != datablock::TYPE_ALLIANCE)
									continue;

								if (block->info() != factionId)
									continue;

								// change icon to green, if alliance status to faction is set
								icon = green;
							}
						}

						// add region only if it has units in it
						if (!region)
							region = appendItem(NULL, new FXRegionItem(regionlabel, terrainIcons[terrain],terrainIcons[terrain], &*block));

						if (icon == blue)
							entry = prependItem(region, new FXRegionItem(label, icon,icon, facPtr));
						else
							entry = appendItem(region, new FXRegionItem(label, icon,icon, facPtr));
					}
				}

				FXTreeItem* act_faction = NULL;
				if (selection.map & selection.ACTIVEFACTION)
				{
					act_faction = factions[selection.activefaction->info()];
				}

				unit = block;
				for (unit++; unit != end && unit->depth() > block->depth(); unit++)
				{
					// display units until next region
					if (unit->type() != datablock::TYPE_UNIT)
						continue;

					// get treeitem node from faction
					FXint factionId = -1;
					FXString fac = unit->value(datakey::TYPE_FACTION);
					if (!fac.empty())
						factionId = strtol(fac.text(), NULL, 10);

					FXTreeItem* faction = factions[factionId];

					FXString name, number;				

					for (datakey::itor key = unit->data().begin(); key != unit->data().end(); key++)
					{
						if (key->type() == datakey::TYPE_NAME)
							name = key->value();
						if (key->type() == datakey::TYPE_NUMBER)
							number = key->value();
					}

					label.format("%s (%s): %s", name.text(), unit->id().text(), number.text());

					// with active_faction_group not set, units of own faction are inserted
					// directly as child of region node.
					FXRegionItem *item;

					if (!active_faction_group && faction == act_faction)
						insertItem(faction, region, item = new FXRegionItem(label, 0,0, &*unit));
					else
						appendItem(faction, item = new FXRegionItem(label, 0,0, &*unit));

					datablock::itor block = unit;
					for (block++; block != end && block->depth() > unit->depth(); block++)
					{
						if (block->type() != datablock::TYPE_COMMANDS)
							continue;

						bool cmd_confirmed = false;

						if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
							cmd_confirmed = cmds->confirmed;

						if (!cmd_confirmed)
						{
							item->setBold(!cmd_confirmed);

							for (FXRegionItem *father = dynamic_cast<FXRegionItem*>(item->getParent());
									father; father = dynamic_cast<FXRegionItem*>(father->getParent()))
								father->setBold(!cmd_confirmed);
						}

					}
				}

				// with active_faction_group not set, remove node of own faction
				if (!active_faction_group && act_faction)
					removeItem(act_faction);
			}
		}
	}

	if (selection.selChange != state->selChange)
	{
		selection.selChange = state->selChange;
		selection.selected = state->selected;
		
		selection.region = state->region;
		selection.faction = state->faction;
		selection.building = state->building;
		selection.ship = state->ship;
		selection.unit = state->unit;

		if (selection.selected & selection.REGION)
		{
			FXTreeItem *item = getFirstItem();
			item = findTreeItem(item, &*selection.region);

			if (selection.selected & selection.FACTION)
				item = findTreeItem(item, &*selection.faction);

			if (selection.selected & selection.BUILDING)
				item = findTreeItem(item, &*selection.building);

			if (selection.selected & selection.SHIP)
				item = findTreeItem(item, &*selection.ship);

			if (selection.selected & selection.UNIT)
				item = findTreeItem(item, &*selection.unit);

			if (item)
			{
				selectItem(item);
				makeItemVisible(item);
			}
		}

		if (selection.selected & selection.UNIT)
		{
			bool cmd_confirmed = true;

			datablock::itor block = selection.unit;
			datablock::itor end = files->front().blocks().end();
			for (block++; block != end && block->depth() > selection.unit->depth(); block++)
			{
				if (block->type() != datablock::TYPE_COMMANDS)
					continue;

				cmd_confirmed = false;

				if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
					cmd_confirmed = cmds->confirmed;
			}

			// all item should be FXRegionItem
			FXRegionItem *item = dynamic_cast<FXRegionItem*>(findTreeItem(getFirstItem(), &*selection.unit));
			if (item)
			{
				item->setBold(!cmd_confirmed);
				update();
			}

			// propagate boldness to parents
			while (FXRegionItem *father = dynamic_cast<FXRegionItem*>(item->getParent()))
			{
				bool boldness = false;
				for (item = dynamic_cast<FXRegionItem*>(father->getFirst()); item; item = dynamic_cast<FXRegionItem*>(item->getNext()))
					if (item->isBold())
						boldness = true;

				father->setBold(boldness);
				item = father;
			}
		}
	}

	return 1;
}

long FXRegionList::onQueryHelp(FXObject* /*sender*/, FXSelector, void* /*ptr*/)
{ 
	// connected to a datafile list?
	if (!files)
		return 0;

	return 0;
}