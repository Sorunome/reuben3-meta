// this file is ISO 8859-1 encoded, or compatible (windows-1252)

#include "language.h"

// general stuff
const MultiLang lang_yes[] = {
	{LANG_EN, "Yes"},
	{LANG_DE, "Ja"},
	{LANG_FR, "Oui"},
};
const MultiLang lang_no[] = {
	{LANG_EN, "No"},
	{LANG_DE, "Nein"},
	{LANG_FR, "Non"},
};
const MultiLang lang_none[] = {
	{LANG_EN, "none"},
	{LANG_DE, "nichts"},
	{LANG_FR, "rien"},
};
const MultiLang lang_hp[] = {
	{LANG_EN, "HP"},
	{LANG_DE, "HP"},
	{LANG_FR, "PV"},
};
const MultiLang lang_mp[] = {
	{LANG_EN, "MP"},
	{LANG_DE, "MP"},
	{LANG_FR, "PM"},
};

// attack menu entries, max 6 chars
const MultiLang lang_battle_menu_attack[] = {
	{LANG_EN, "Attack"},
	{LANG_DE, "Angrif"},
	{LANG_FR, "Attaqu"},
};
const MultiLang lang_battle_menu_magic[] = {
	{LANG_EN, "Magic"},
	{LANG_DE, "Magie"},
	{LANG_FR, "Magie"},
};
const MultiLang lang_battle_menu_item[] = {
	{LANG_EN, "Item"},
	{LANG_DE, "Item"},
	{LANG_FR, "Objets"},
};
const MultiLang lang_battle_menu_run[] = {
	{LANG_EN, "Run"},
	{LANG_DE, "Fliehe"},
	{LANG_FR, "Fuir"},
};

// magic menu entries, max 6 chars
const MultiLang lang_battle_magic_fire[] = {
	{LANG_EN, "Fire"},
	{LANG_DE, "Feuer"},
	{LANG_FR, "Feu"},
};
const MultiLang lang_battle_magic_ice[] = {
	{LANG_EN, "Ice"},
	{LANG_DE, "Eis"},
	{LANG_FR, "Glace"},
};
const MultiLang lang_battle_magic_bolt[] = {
	{LANG_EN, "Bolt"},
	{LANG_DE, "Sturm"},
	{LANG_FR, "Éclair"},
};
const MultiLang lang_battle_magic_stun[] = {
	{LANG_EN, "Stun"},
	{LANG_DE, "Lähmen"},
	{LANG_FR, "Bloque"},
};

// main menu stuff
const MultiLang lang_menu_delete_savegame[] = {
	{LANG_EN, "Delete savegame?"},
	{LANG_DE, "Spielstand löschen?"},
	{LANG_FR, "Supprimer partie?"},
};
const MultiLang lang_menu_load_game[] = {
	{LANG_EN, "Load Game"},
	{LANG_DE, "Lade Spiel"},
	{LANG_FR, "Charger partie"},
};
const MultiLang lang_menu_empty_slot[] = {
	{LANG_EN, "-empty slot-"},
	{LANG_DE, "-neues Spiel-"},
	{LANG_FR, "-bloc vide-"},
};
const MultiLang lang_menu_level[] = {
	{LANG_EN, "Level "},
	{LANG_DE, "Level "},
	{LANG_FR, "Niveau "},
};
const MultiLang lang_menu_start[] = {
	{LANG_EN, "Start"},
	{LANG_DE, "Starte"},
	{LANG_FR, "Commencer"},
};
const MultiLang lang_menu_instructions[] = {
	{LANG_EN, "Instructions"},
	{LANG_DE, "Anleitung"},
	{LANG_FR, "Instructions"},
};
const MultiLang lang_menu_credits[] = {
	{LANG_EN, "Credits"},
	{LANG_DE, "Mitwirkende"},
	{LANG_FR, "Crédits"},
};


// bottle strings. Max length 8 chars
const MultiLang lang_bottle_bottle[] = {
	{LANG_EN, "Bottle"},
	{LANG_DE, "Flasche"},
	{LANG_FR, "Fiole"},
};
const MultiLang lang_bottle_badwater[] = {
	{LANG_EN, "badWater"},
	{LANG_DE, "Dreckwsr"},
	{LANG_FR, "Eau sale"},
};
const MultiLang lang_bottle_water[] = {
	{LANG_EN, "Water"},
	{LANG_DE, "Wasser"},
	{LANG_FR, "Eau"},
};
const MultiLang lang_bottle_herb[] = {
	{LANG_EN, "Herb"},
	{LANG_DE, "Pflanze"},
	{LANG_FR, "Plante"},
};
const MultiLang lang_bottle_potion[] = {
	{LANG_EN, "Potion"},
	{LANG_DE, "Trank"},
	{LANG_FR, "Potion"},
};
const MultiLang lang_bottle_ginseng[] = {
	{LANG_EN, "Ginseng"},
	{LANG_DE, "Ginseng"},
	{LANG_FR, "Ginseng"},
};
const MultiLang lang_bottle_elixir[] = {
	{LANG_EN, "Elixir"},
	{LANG_DE, "Elixir"},
	{LANG_FR, "Élixir"},
};

// area strings and "Area:" togeather max 14+5=19 chars
const MultiLang lang_area_dogglen[] = {
	{LANG_EN, "Dogglen"},
	{LANG_DE, "Dogglen"},
	{LANG_FR, "Dogglen"},
};
const MultiLang lang_area_forest[] = {
	{LANG_EN, "Forest"},
	{LANG_DE, "Wald"},
	{LANG_FR, "Forêt"},
};
const MultiLang lang_area_east_mountains[] = {
	{LANG_EN, "East Mountains"},
	{LANG_DE, "Ostgebierge"},
	{LANG_FR, "Montagnes Est"},
};
const MultiLang lang_area_aerilon[] = {
	{LANG_EN, "Aerilon"},
	{LANG_DE, "Aerilon"},
	{LANG_FR, "Aerilon"},
};
const MultiLang lang_area_ugana_plain[] = {
	{LANG_EN, "Ugana Plain"},
	{LANG_DE, "Ugana Ebene"},
	{LANG_FR, "Plaine d'Ugana"},
};
const MultiLang lang_area_aerilon_castle[] = {
	{LANG_EN, "Aerilon Castle"},
	{LANG_DE, "Burg Aerilon"},
	{LANG_FR, "ChâteauAerilon"},
};
const MultiLang lang_area_eastern_peaks[] = {
	{LANG_EN, "Eastern Peaks"},
	{LANG_DE, "Ostgipfel"},
	{LANG_FR, "Pics Est"},
};
const MultiLang lang_area_fragile_caves[] = {
	{LANG_EN, "Fragile Caves"},
	{LANG_DE, "Brüchige Höhle"},
	{LANG_FR, "Grotte fragile"},
};
const MultiLang lang_area_gnah_lake[] = {
	{LANG_EN, "Gnah Lake"},
	{LANG_DE, "See Gnah"},
	{LANG_FR, "Lac Gnah"},
};
const MultiLang lang_area_caves[] = {
	{LANG_EN, "Caves"},
	{LANG_DE, "Höhlen"},
	{LANG_FR, "Grottes"},
};
const MultiLang lang_area_cliff_break[] = {
	{LANG_EN, "Cliff-Break"},
	{LANG_DE, "Cliff-Break"},
	{LANG_FR, "Falaise"},
};
const MultiLang lang_area_antique_castle[] = {
	{LANG_EN, "Antique Castle"},
	{LANG_DE, "Alte Burg"},
	{LANG_FR, "Vieux château"},
};
const MultiLang lang_area_desert[] = {
	{LANG_EN, "Desert"},
	{LANG_DE, "Wüste"},
	{LANG_FR, "Désert"},
};
const MultiLang lang_area_castle_present[] = {
	{LANG_EN, "Castle Present"},
	{LANG_DE, "Burg heute"},
	{LANG_FR, "ChâteauPrésent"},
};
const MultiLang lang_area_castle_past[] = {
	{LANG_EN, "Castle Past"},
	{LANG_DE, "Burg früher"},
	{LANG_FR, "Château Passé"},
};


// weapons strings. Max 9 chars
const MultiLang lang_weapon_stick[] = {
	{LANG_EN, "Stick"},
	{LANG_DE, "Stock"},
	{LANG_FR, "Bâton"},
};
const MultiLang lang_weapon_axe[] = {
	{LANG_EN, "Axe"},
	{LANG_DE, "Axt"},
	{LANG_FR, "Hache"},
};
const MultiLang lang_weapon_sword[] = {
	{LANG_EN, "Sword"},
	{LANG_DE, "Schwert"},
	{LANG_FR, "Épée"},
};
const MultiLang lang_weapon_breadsword[] = {
	{LANG_EN, "BreadSwrd"},
	{LANG_DE, "HartSwrt"},
	{LANG_FR, "Épée dure"},
};
const MultiLang lang_weapon_toxicsword[] = {
	{LANG_EN, "ToxicSwrd"},
	{LANG_DE, "GiftSwrt"},
	{LANG_FR, "ÉpéeToxic"},
};

// armor strings. Max 9 chars
const MultiLang lang_armor_wounds[] = {
	{LANG_EN, "Wounds"},
	{LANG_DE, "Wunden"},
	{LANG_FR, "Blessures"},
};
const MultiLang lang_armor_shield[] = {
	{LANG_EN, "Shield"},
	{LANG_DE, "Schilt"},
	{LANG_FR, "Bouclier"},
};

// trad equest strings. Max 9 chars
const MultiLang lang_trade_letter[] = {
	{LANG_EN, "Letter"},
	{LANG_DE, "Brief"},
	{LANG_FR, "Lettre"},
};
const MultiLang lang_trade_letter_2[] = {
	{LANG_EN, "Letter 2"},
	{LANG_DE, "Brief 2"},
	{LANG_FR, "Lettre 2"},
};
const MultiLang lang_trade_food[] = {
	{LANG_EN, "Food"},
	{LANG_DE, "Essen"},
	{LANG_FR, "Aliments"},
};
const MultiLang lang_trade_fish_rod[] = {
	{LANG_EN, "Fish Rod"},
	{LANG_DE, "Angel"},
	{LANG_FR, "CannePêch"},
};
const MultiLang lang_trade_fish[] = {
	{LANG_EN, "Fish"},
	{LANG_DE, "Fisch"},
	{LANG_FR, "Poisson"},
};
const MultiLang lang_trade_necklace[] = {
	{LANG_EN, "Necklace"},
	{LANG_DE, "Halskette"},
	{LANG_FR, "Collier"},
};
const MultiLang lang_trade_shell[] = {
	{LANG_EN, "Shell"},
	{LANG_DE, "Muschel"},
	{LANG_FR, "Coquille"},
};
const MultiLang lang_trade_rug[] = {
	{LANG_EN, "Rug"},
	{LANG_DE, "Teppich"},
	{LANG_FR, "Tapis"},
};
const MultiLang lang_trade_fox[] = {
	{LANG_EN, "Fox"},
	{LANG_DE, "Fuchs"},
	{LANG_FR, "Renard"},
};

// stats page each individual max len
// max 6 chars
const MultiLang lang_stats_gold[] = {
	{LANG_EN, "Gold:"},
	{LANG_DE, "Gold:"},
	{LANG_FR, "Or:"},
};
// max 7 chars
const MultiLang lang_stats_scare[] = {
	{LANG_EN, "Scare:"},
	{LANG_DE, "Furcht:"},
	{LANG_FR, "Peur:"},
};
// max 7 chars
const MultiLang lang_stats_level[] = {
	{LANG_EN, "Level:"},
	{LANG_DE, "Level:"},
	{LANG_FR, "Niveau:"},
};
// max 9 chars
const MultiLang lang_stats_weapon[] = {
	{LANG_EN, "Weapon:"},
	{LANG_DE, "Waffe:"},
	{LANG_FR, "Arme:"},
};
// max 9 chars
const MultiLang lang_stats_amror[] = {
	{LANG_EN, "Armor:"},
	{LANG_DE, "Rüstung:"},
	{LANG_FR, "Armure:"},
};
// max 6 chars
const MultiLang lang_stats_speed[] = {
	{LANG_EN, "Speed:"},
	{LANG_DE, "Tempo:"},
	{LANG_FR, "Vit:"},
};
// max 9 chars
const MultiLang lang_stats_next_exp[] = {
	{LANG_EN, "Next EXP:"},
	{LANG_DE, "Noch EXP:"},
	{LANG_FR, "ProchEXP:"},
};
// max 9 chars
const MultiLang lang_stats_tradeitem[] = {
	{LANG_EN, "TradeItm:"},
	{LANG_DE, "HandlObj:"},
	{LANG_FR, "Obj Troc:"},
};

// item strings. Max length 8 chars
const MultiLang lang_item_hookshot[] = {
	{LANG_EN, "Hookshot"},
	{LANG_DE, "Hookshot"},
	{LANG_FR, "Grapin"},
};
const MultiLang lang_item_bombs[] = { // max length 6 chars
	{LANG_EN, "Bombs:"},
	{LANG_DE, "Bombe:"},
	{LANG_FR, "Bombe:"},
};
const MultiLang lang_item_protect[] = {
	{LANG_EN, "Protect"},
	{LANG_DE, "Schutz"},
	{LANG_FR, "Protéger"},
};

// misc area, see condition with area strings
const MultiLang lang_area[] = {
	{LANG_EN, "Area:"},
	{LANG_DE, "Ort:"},
	{LANG_FR, "Zone:"},
};

// instruction strings
const MultiLang lang_instruction_move[] = {
	{LANG_EN, "Move"},
	{LANG_DE, "Bewegen"},
	{LANG_FR, "Bouger"},
};
const MultiLang lang_instructions_interact[] = {
	{LANG_EN, "Interact"},
	{LANG_DE, "Interagieren"},
	{LANG_FR, "Interagir"},
};
const MultiLang lang_instructions_use_item[] = {
	{LANG_EN, "Use item"},
	{LANG_DE, "Verwende Item"},
	{LANG_FR, "Utiliser obj."},
};
const MultiLang lang_instructions_stats_menu[] = {
	{LANG_EN, "Stats menu"},
	{LANG_DE, "Statusmenü"},
	{LANG_FR, "Menu stats"},
};
