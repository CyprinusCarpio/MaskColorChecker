// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "MaskColorCheckerGUI.h"
#include "Program.h"

Fl_Spinner *g_overlapSpinner=(Fl_Spinner *)0;

static void cb_g_overlapSpinner(Fl_Spinner*, void*) {
  OnGui(CB_OVERLAP);
}

Fl_Spinner *g_colorsPerTileSpinner=(Fl_Spinner *)0;

static void cb_g_colorsPerTileSpinner(Fl_Spinner*, void*) {
  OnGui(CB_COLORSPERTILE);
}

Fl_Check_Button *g_solidFillCheckButton=(Fl_Check_Button *)0;

static void cb_g_solidFillCheckButton(Fl_Check_Button*, void*) {
  OnGui(CB_SOLIDFILL);
}

Fl_Button *g_badTileFillColorButton=(Fl_Button *)0;

static void cb_g_badTileFillColorButton(Fl_Button*, void*) {
  OnGui(CB_BADTILECOLOR);
}

Fl_Output *g_maskPathOutput=(Fl_Output *)0;

Fl_Button *g_loadMaskButton=(Fl_Button *)0;

static void cb_g_loadMaskButton(Fl_Button*, void*) {
  OnGui(CB_LOADMASK);
}

Fl_Button *g_checkButton=(Fl_Button *)0;

static void cb_g_checkButton(Fl_Button*, void*) {
  OnGui(CB_CHECK);
}

Fl_Button *g_saveOutputButton=(Fl_Button *)0;

static void cb_g_saveOutputButton(Fl_Button*, void*) {
  OnGui(CB_SAVEOUTPUT);
}

Fl_Button *g_gridColorButton=(Fl_Button *)0;

static void cb_g_gridColorButton(Fl_Button*, void*) {
  OnGui(CB_GRIDCOLOR);
}

Fl_Check_Button *g_drawGridCheckButton=(Fl_Check_Button *)0;

static void cb_g_drawGridCheckButton(Fl_Check_Button*, void*) {
  OnGui(CB_DRAWGRID);
}

Fl_Box *g_parametersStatusBox=(Fl_Box *)0;

#include <FL/Fl_Pixmap.H>

static const char *idata_invalidParameters[] = {
"22 22 103 2",
"  \tc None",
". \tc #0B0000",
"+ \tc #4F0000",
"@ \tc #410000",
"# \tc #320000",
"$ \tc #5C0000",
"% \tc #A10000",
"& \tc #560000",
"* \tc #6B0000",
"= \tc #5E0000",
"- \tc #540000",
"; \tc #F20000",
"> \tc #C90000",
", \tc #460000",
"\' \tc #530000",
") \tc #650000",
"! \tc #790000",
"~ \tc #B40000",
"{ \tc #BD0000",
"] \tc #A60000",
"^ \tc #810000",
"/ \tc #AC0000",
"( \tc #FF0000",
"_ \tc #DA0000",
": \tc #390000",
"< \tc #4D0000",
"[ \tc #6E0000",
"} \tc #9E0000",
"| \tc #F10000",
"1 \tc #B10000",
"2 \tc #780000",
"3 \tc #2B0000",
"4 \tc #2C0000",
"5 \tc #1E0000",
"6 \tc #9F0000",
"7 \tc #AF0000",
"8 \tc #7E0000",
"9 \tc #4B0000",
"0 \tc #8F0000",
"a \tc #400000",
"b \tc #260000",
"c \tc #130000",
"d \tc #A80000",
"e \tc #AA0000",
"f \tc #640000",
"g \tc #8B0000",
"h \tc #970000",
"i \tc #3D0000",
"j \tc #360000",
"k \tc #A30000",
"l \tc #DC0000",
"m \tc #C50000",
"n \tc #3C0000",
"o \tc #570000",
"p \tc #660000",
"q \tc #EF0000",
"r \tc #8E0000",
"s \tc #860000",
"t \tc #270000",
"u \tc #340000",
"v \tc #830000",
"w \tc #2A0000",
"x \tc #880000",
"y \tc #850000",
"z \tc #EC0000",
"A \tc #A50000",
"B \tc #420000",
"C \tc #610000",
"D \tc #6D0000",
"E \tc #F90000",
"F \tc #8A0000",
"G \tc #150000",
"H \tc #740000",
"I \tc #5D0000",
"J \tc #9A0000",
"K \tc #510000",
"L \tc #A70000",
"M \tc #BF0000",
"N \tc #C60000",
"O \tc #220000",
"P \tc #9B0000",
"Q \tc #600000",
"R \tc #7F0000",
"S \tc #800000",
"T \tc #4A0000",
"U \tc #470000",
"V \tc #230000",
"W \tc #CF0000",
"X \tc #710000",
"Y \tc #300000",
"Z \tc #E60000",
"` \tc #3A0000",
" .\tc #580000",
"..\tc #8C0000",
"+.\tc #350000",
"@.\tc #7B0000",
"#.\tc #A90000",
"$.\tc #330000",
"%.\tc #2F0000",
"&.\tc #620000",
"*.\tc #370000",
"=.\tc #240000",
"-.\tc #870000",
"                                            ",
"  .                                 + @     ",
"  # $ +                         % & * =     ",
"  - ; > ,                   \' ) ! ~ { ]     ",
"  ^ / ( _ & :             < [ } ( | 1       ",
"    * _ ( ( 2 3       4 5 6 7 ( ( 8         ",
"      9 2 ( ( 0 a b c 3 d 7 ( e f           ",
"        a g ( ( h i j k l ( m n             ",
"          o p ( ( q r ( ( s t               ",
"            u q ( ( ( ( v w                 ",
"              x y z ( A B                   ",
"              C D E ; F p G                 ",
"            H I J ( ( ( ( a #               ",
"            K L ( M * ( ( N O               ",
"          , P ( ( Q   + ; ( R b             ",
"        p + ( ( S       T ( ( & -           ",
"      U V ; W X           , ( W t           ",
"      Y d Z K               \' ; r `         ",
"    #  ...+.                @.9 #.$.        ",
"    %.&.*.                    K b =.y       ",
"      3                         I -.        ",
"                                            "
};
static Fl_Image *image_invalidParameters() {
  static Fl_Image *image = NULL;
  if (!image)
    image = new Fl_Pixmap(idata_invalidParameters);
  return image;
}

static const char *idata_validParameters[] = {
"22 22 72 1",
" \tc None",
".\tc #004C00",
"+\tc #000B00",
"@\tc #00EC00",
"#\tc #00C200",
"$\tc #002C00",
"%\tc #003D00",
"&\tc #00CB00",
"*\tc #009300",
"=\tc #005000",
"-\tc #00A400",
";\tc #00D100",
">\tc #00FF00",
",\tc #003E00",
"\'\tc #00B900",
")\tc #00DC00",
"!\tc #00F200",
"~\tc #006900",
"{\tc #001C00",
"]\tc #00E800",
"^\tc #00AC00",
"/\tc #007200",
"(\tc #008800",
"_\tc #00AF00",
":\tc #004200",
"<\tc #00EA00",
"[\tc #007600",
"}\tc #004A00",
"|\tc #00D800",
"1\tc #00BC00",
"2\tc #004000",
"3\tc #002300",
"4\tc #008200",
"5\tc #00F100",
"6\tc #008400",
"7\tc #006A00",
"8\tc #00B700",
"9\tc #008500",
"0\tc #005600",
"a\tc #009000",
"b\tc #007B00",
"c\tc #00AB00",
"d\tc #00BD00",
"e\tc #00B500",
"f\tc #007E00",
"g\tc #006C00",
"h\tc #004E00",
"i\tc #004800",
"j\tc #00DA00",
"k\tc #00A600",
"l\tc #008F00",
"m\tc #005900",
"n\tc #00CC00",
"o\tc #004600",
"p\tc #005400",
"q\tc #009D00",
"r\tc #008100",
"s\tc #006000",
"t\tc #007300",
"u\tc #00D200",
"v\tc #00B400",
"w\tc #008C00",
"x\tc #003F00",
"y\tc #003400",
"z\tc #002A00",
"A\tc #005F00",
"B\tc #00E600",
"C\tc #003000",
"D\tc #006300",
"E\tc #001800",
"F\tc #006700",
"G\tc #004B00",
"                      ",
"                   .+ ",
"                 @#$% ",
"                &#*=  ",
"               -;>,   ",
"              \')!~{   ",
" ]^/         (!>_:    ",
" <)[}        |>123    ",
" 45!6/     78>!90     ",
"  a>>b     c>>d}      ",
"   e>>f   g>>!hi      ",
"    j>>h 2k>>lm       ",
"     d>no >>!p        ",
"     q)>rs>>/t        ",
"      u>>>>_,v        ",
"      wd>>>xy         ",
"       q>>;zA         ",
"       ldBC,          ",
"        #DEA          ",
"        FGo           ",
"                      ",
"                      "
};
static Fl_Image *image_validParameters() {
  static Fl_Image *image = NULL;
  if (!image)
    image = new Fl_Pixmap(idata_validParameters);
  return image;
}

Fl_Choice *g_tileSizeChoice=(Fl_Choice *)0;

static void cb_g_tileSizeChoice(Fl_Choice*, void*) {
  OnGui(CB_TILESIZE);
}

Fl_Spinner *g_tilesInRowSpinner=(Fl_Spinner *)0;

static void cb_g_tilesInRowSpinner(Fl_Spinner*, void*) {
  OnGui(CB_TILECOUNT);
}

Fl_Button *g_fixButton=(Fl_Button *)0;

static void cb_g_fixButton(Fl_Button*, void*) {
  OnGui(CB_FIX);
}

Fl_Button *g_saveGridButton=(Fl_Button *)0;

static void cb_g_saveGridButton(Fl_Button*, void*) {
  OnGui(CB_SAVEGRID);
}

Canvas *g_canvas=(Canvas *)0;

Fl_Menu_Bar *g_menuBar=(Fl_Menu_Bar *)0;

static void cb_fileopen(Fl_Menu_*, void*) {
  OnGui(CB_OPEN);
}

static void cb_g_menuItemSave(Fl_Menu_*, void*) {
  OnGui(CB_SAVE);
}

static void cb_g_menuItemSaveAs(Fl_Menu_*, void*) {
  OnGui(CB_SAVEAS);
}

static void cb_base(Fl_Menu_*, void*) {
  OnGui(CB_THEME_BASE);
}

static void cb_plastic(Fl_Menu_*, void*) {
  OnGui(CB_THEME_PLASTIC);
}

static void cb_gtk(Fl_Menu_*, void*) {
  OnGui(CB_THEME_GTK);
}

static void cb_gleam(Fl_Menu_*, void*) {
  OnGui(CB_THEME_GLEAM);
}

static void cb_oxy(Fl_Menu_*, void*) {
  OnGui(CB_THEME_OXY);
}

static void cb_Auto(Fl_Menu_*, void*) {
  OnGui(CB_COLORS_AUTO);
}

static void cb_Dark(Fl_Menu_*, void*) {
  OnGui(CB_COLORS_DARK);
}

static void cb_Light(Fl_Menu_*, void*) {
  OnGui(CB_COLORS_LIGHT);
}

static void cb_System(Fl_Menu_*, void*) {
  OnGui(CB_COLORS_SYSTEM);
}

static void cb_Help(Fl_Menu_*, void*) {
  OnGui(CB_HELP);
}

static void cb_About(Fl_Menu_*, void*) {
  OnGui(CB_ABOUT);
}

Fl_Menu_Item menu_g_menuBar[] = {
 {"Project", 0,  0, 0, 64, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"@fileopen Open", FL_CTRL|'o',  (Fl_Callback*)cb_fileopen, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"@filesave Save", FL_CTRL|'s',  (Fl_Callback*)cb_g_menuItemSave, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"@filesaveas Save as", 0,  (Fl_Callback*)cb_g_menuItemSaveAs, 0, 128, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Theme", 0,  0, 0, 64, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"base", 0,  (Fl_Callback*)cb_base, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"plastic", 0,  (Fl_Callback*)cb_plastic, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"gtk+", 0,  (Fl_Callback*)cb_gtk, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"gleam", 0,  (Fl_Callback*)cb_gleam, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"oxy", 0,  (Fl_Callback*)cb_oxy, 0, 136, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Auto colors", 0,  (Fl_Callback*)cb_Auto, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Dark colors", 0,  (Fl_Callback*)cb_Dark, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Light colors", 0,  (Fl_Callback*)cb_Light, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"System colors", 0,  (Fl_Callback*)cb_System, 0, 8, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"About", 0,  0, 0, 64, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"@. Help  ", 0,  (Fl_Callback*)cb_Help, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"@. About  ", 0,  (Fl_Callback*)cb_About, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Double_Window* make_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(400, 350, "Mask Color Checker v0.2");
    w = o; (void)w;
    { Fl_Group* o = new Fl_Group(0, 20, 400, 330);
      { Fl_Group* o = new Fl_Group(0, 20, 180, 330, "Mask Color Checker v0.2");
        o->box(FL_UP_BOX);
        o->labelfont(3);
        o->labelsize(15);
        o->align(Fl_Align(FL_ALIGN_TOP|FL_ALIGN_INSIDE));
        { g_overlapSpinner = new Fl_Spinner(115, 85, 60, 20, "Actual overlap:");
          g_overlapSpinner->tooltip("Mapframe properties > Samplers > Actual overlap (px)");
          g_overlapSpinner->minimum(0);
          g_overlapSpinner->maximum(512);
          g_overlapSpinner->value(0);
          g_overlapSpinner->callback((Fl_Callback*)cb_g_overlapSpinner);
          g_overlapSpinner->when(FL_WHEN_CHANGED);
        } // Fl_Spinner* g_overlapSpinner
        { g_colorsPerTileSpinner = new Fl_Spinner(115, 130, 60, 20, "Colors per tile:");
          g_colorsPerTileSpinner->tooltip("How many colors are allowed in a tile?");
          g_colorsPerTileSpinner->minimum(4);
          g_colorsPerTileSpinner->maximum(6);
          g_colorsPerTileSpinner->value(5);
          g_colorsPerTileSpinner->callback((Fl_Callback*)cb_g_colorsPerTileSpinner);
          g_colorsPerTileSpinner->when(FL_WHEN_CHANGED);
        } // Fl_Spinner* g_colorsPerTileSpinner
        { g_solidFillCheckButton = new Fl_Check_Button(97, 221, 75, 15, "Solid fill");
          g_solidFillCheckButton->tooltip("Whether to use solid color to indicate bad tiles");
          g_solidFillCheckButton->down_box(FL_DOWN_BOX);
          g_solidFillCheckButton->callback((Fl_Callback*)cb_g_solidFillCheckButton);
          g_solidFillCheckButton->when(FL_WHEN_CHANGED);
        } // Fl_Check_Button* g_solidFillCheckButton
        { g_badTileFillColorButton = new Fl_Button(5, 218, 90, 22, "Bad tile color");
          g_badTileFillColorButton->tooltip("This color will be used to draw bad tile indicators.");
          g_badTileFillColorButton->box(FL_DOWN_BOX);
          g_badTileFillColorButton->color((Fl_Color)1);
          g_badTileFillColorButton->callback((Fl_Callback*)cb_g_badTileFillColorButton);
        } // Fl_Button* g_badTileFillColorButton
        { g_maskPathOutput = new Fl_Output(5, 185, 170, 25, "Mask path:");
          g_maskPathOutput->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        } // Fl_Output* g_maskPathOutput
        { g_loadMaskButton = new Fl_Button(80, 155, 95, 25, "Load mask    @fileopen");
          g_loadMaskButton->tooltip("Load mask image to be checked");
          g_loadMaskButton->labelsize(12);
          g_loadMaskButton->callback((Fl_Callback*)cb_g_loadMaskButton);
        } // Fl_Button* g_loadMaskButton
        { g_checkButton = new Fl_Button(5, 320, 65, 25, "Check @->");
          g_checkButton->tooltip("Start the check. This may hang the program for a while.");
          g_checkButton->labelsize(12);
          g_checkButton->callback((Fl_Callback*)cb_g_checkButton);
          g_checkButton->window()->hotspot(g_checkButton);
        } // Fl_Button* g_checkButton
        { g_saveOutputButton = new Fl_Button(75, 320, 100, 25, "Save bad tiles @filesave");
          g_saveOutputButton->tooltip("Save the output faulty tiles image");
          g_saveOutputButton->labelsize(12);
          g_saveOutputButton->callback((Fl_Callback*)cb_g_saveOutputButton);
          g_saveOutputButton->deactivate();
        } // Fl_Button* g_saveOutputButton
        { Fl_Box* o = new Fl_Box(0, 280, 180, 5);
          Fl_Group::current()->resizable(o);
        } // Fl_Box* o
        { g_gridColorButton = new Fl_Button(5, 250, 90, 22, "Grid color");
          g_gridColorButton->tooltip("This color will be used to draw a grid marking the tile borders");
          g_gridColorButton->box(FL_DOWN_BOX);
          g_gridColorButton->color((Fl_Color)6);
          g_gridColorButton->callback((Fl_Callback*)cb_g_gridColorButton);
        } // Fl_Button* g_gridColorButton
        { g_drawGridCheckButton = new Fl_Check_Button(97, 254, 80, 15, "Draw grid");
          g_drawGridCheckButton->tooltip("Whether to draw the grid");
          g_drawGridCheckButton->down_box(FL_DOWN_BOX);
          g_drawGridCheckButton->callback((Fl_Callback*)cb_g_drawGridCheckButton);
          g_drawGridCheckButton->when(FL_WHEN_CHANGED);
        } // Fl_Check_Button* g_drawGridCheckButton
        { g_parametersStatusBox = new Fl_Box(10, 40, 165, 22, "Invalid parameters");
          g_parametersStatusBox->image( image_invalidParameters() );
          g_parametersStatusBox->deimage( image_validParameters() );
          g_parametersStatusBox->align(Fl_Align(256));
        } // Fl_Box* g_parametersStatusBox
        { g_tileSizeChoice = new Fl_Choice(115, 62, 60, 20, "Tile size:");
          g_tileSizeChoice->tooltip("Mapframe properties > Samplers > Satellite/Surface (mask) tiles > Size (px)");
          g_tileSizeChoice->down_box(FL_BORDER_BOX);
          g_tileSizeChoice->callback((Fl_Callback*)cb_g_tileSizeChoice);
          g_tileSizeChoice->when(FL_WHEN_CHANGED);
          g_tileSizeChoice->add("128|256|512|1024|2048");
          g_tileSizeChoice->value(1);
        } // Fl_Choice* g_tileSizeChoice
        { g_tilesInRowSpinner = new Fl_Spinner(115, 108, 60, 20, "Tiles in row:");
          g_tilesInRowSpinner->tooltip("Mapframe properties > Samplers > Tiles in row (tile)");
          g_tilesInRowSpinner->callback((Fl_Callback*)cb_g_tilesInRowSpinner);
          g_tilesInRowSpinner->when(FL_WHEN_CHANGED);
        } // Fl_Spinner* g_tilesInRowSpinner
        { g_fixButton = new Fl_Button(5, 290, 65, 25, "Fix        @redo");
          g_fixButton->tooltip("Heuristically fix the mask and save it to a different location");
          g_fixButton->labelsize(12);
          g_fixButton->callback((Fl_Callback*)cb_g_fixButton);
          g_fixButton->deactivate();
        } // Fl_Button* g_fixButton
        { g_saveGridButton = new Fl_Button(75, 290, 100, 25, "Save grid         @filesave");
          g_saveGridButton->tooltip("Save the grid to a image");
          g_saveGridButton->labelsize(12);
          g_saveGridButton->callback((Fl_Callback*)cb_g_saveGridButton);
          g_saveGridButton->deactivate();
        } // Fl_Button* g_saveGridButton
        o->end();
      } // Fl_Group* o
      { g_canvas = new Canvas(180, 20, 220, 330);
        g_canvas->box(FL_NO_BOX);
        g_canvas->color(FL_BACKGROUND_COLOR);
        g_canvas->selection_color(FL_BACKGROUND_COLOR);
        g_canvas->labeltype(FL_NORMAL_LABEL);
        g_canvas->labelfont(0);
        g_canvas->labelsize(14);
        g_canvas->labelcolor(FL_FOREGROUND_COLOR);
        g_canvas->align(Fl_Align(FL_ALIGN_CENTER));
        g_canvas->when(FL_WHEN_RELEASE);
        Fl_Group::current()->resizable(g_canvas);
      } // Canvas* g_canvas
      o->end();
      Fl_Group::current()->resizable(o);
    } // Fl_Group* o
    { g_menuBar = new Fl_Menu_Bar(0, 0, 400, 20);
      g_menuBar->labelsize(12);
      g_menuBar->menu(menu_g_menuBar);
    } // Fl_Menu_Bar* g_menuBar
    o->size_range(400, 350);
    o->end();
  } // Fl_Double_Window* o
  return w;
}
