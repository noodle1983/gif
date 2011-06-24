#ifndef GIF_H
#define GIF_H

#include <stdint.h>
using namespace std;

#pragma   pack(1)

/**
 *15. Data Sub-blocks.
 *
 *      a. Description. Data Sub-blocks are units containing data. They do not
 *      have a label, these blocks are processed in the context of control
 *      blocks, wherever data blocks are specified in the format. The first byte
 *      of the Data sub-block indicates the number of data bytes to follow. A
 *      data sub-block may contain from 0 to 255 data bytes. The size of the
 *      block does not account for the size byte itself, therefore, the empty
 *      sub-block is one whose size field contains 0x00.
 *
 *      b. Required Version.  87a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Block Size                    Byte
 *     +---------------+
 *  1  |               |
 *     +-             -+
 *  2  |               |
 *     +-             -+
 *  3  |               |
 *     +-             -+
 *     |               |       Data Values                   Byte
 *     +-             -+
 * up  |               |
 *     +-   . . . .   -+
 * to  |               |
 *     +-             -+
 *     |               |
 *     +-             -+
 *255  |               |
 *     +---------------+
 *
 *            i) Block Size - Number of bytes in the Data Sub-block; the size
 *            must be within 0 and 255 bytes, inclusive.
 *
 *            ii) Data Values - Any 8-bit value. There must be exactly as many
 *            Data Values as specified by the Block Size field.
 *
 *      d. Extensions and Scope. This type of block always occurs as part of a
 *      larger unit. It does not have a scope of itself.
 *
 *      e. Recommendation. None.
 *
 *
 *16. Block Terminator.
 *
 *      a. Description. This zero-length Data Sub-block is used to terminate a
 *      sequence of Data Sub-blocks. It contains a single byte in the position of
 *      the Block Size field and does not contain data.
 *
 *      b. Required Version.  87a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Block Size                    Byte
 *     +---------------+
 *
 *            i) Block Size - Number of bytes in the Data Sub-block; this field
 *            contains the fixed value 0x00.
 *
 *            ii) Data Values - This block does not contain any data.
 *
 *      d. Extensions and Scope. This block terminates the immediately preceding
 *      sequence of Data Sub-blocks. This block cannot be modified by any
 *      extension.
 *
 *      e. Recommendation. None.
 */

typedef struct 
{
   unsigned char block_size;
   char data_value[256];
   unsigned char block_terminator;
}gif_data_sub_block_ter_t;

/**
 * 17. Header.
 *
 *      a. Description. The Header identifies the GIF Data Stream in context. The
 *      Signature field marks the beginning of the Data Stream, and the Version
 *      field identifies the set of capabilities required of a decoder to fully
 *      process the Data Stream.  This block is REQUIRED; exactly one Header must
 *      be present per Data Stream.
 *
 *      b. Required Version.  Not applicable. This block is not subject to a
 *      version number. This block must appear at the beginning of every Data
 *      Stream.
 *
 *      c. Syntax.
 *
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *   0 |               |       Signature                     3 Bytes
 *     +-             -+
 *   1 |               |
 *     +-             -+
 *   2 |               |
 *     +---------------+
 *   3 |               |       Version                       3 Bytes
 *     +-             -+
 *   4 |               |
 *     +-             -+
 *   5 |               |
 *     +---------------+
 *
 *            i) Signature - Identifies the GIF Data Stream. This field contains
 *            the fixed value 'GIF'.
 *
 *            ii) Version - Version number used to format the data stream.
 *            Identifies the minimum set of capabilities necessary to a decoder
 *            to fully process the contents of the Data Stream.
 *
 *            Version Numbers as of 10 July 1990 :       "87a" - May 1987
 *                                                       "89a" - July 1989
 *
 *            Version numbers are ordered numerically increasing on the first two
 *            digits starting with 87 (87,88,...,99,00,...,85,86) and
 *            alphabetically increasing on the third character (a,...,z).
 *
 *            iii) Extensions and Scope. The scope of this block is the entire
 *            Data Stream. This block cannot be modified by any extension.
 *
 *      d. Recommendations.
 *
 *            i) Signature - This field identifies the beginning of the GIF Data
 *            Stream; it is not intended to provide a unique signature for the
 *            identification of the data. It is recommended that the GIF Data
 *            Stream be identified externally by the application. (Refer to
 *            Appendix G for on-line identification of the GIF Data Stream.)
 *
 *            ii) Version - ENCODER : An encoder should use the earliest possible
 *            version number that defines all the blocks used in the Data Stream.
 *            When two or more Data Streams are combined, the latest of the
 *            individual version numbers should be used for the resulting Data
 *            Stream. DECODER : A decoder should attempt to process the data
 *            stream to the best of its ability; if it encounters a version
 *            number which it is not capable of processing fully, it should
 *            nevertheless, attempt to process the data stream to the best of its
 *            ability, perhaps after warning the user that the data may be
 *            incomplete.
 **/
typedef struct 
{
   char signature[3];
   char version[3];
}gif_header_t;

/**
 * 18. Logical Screen Descriptor.
 *
 *      a. Description.  The Logical Screen Descriptor contains the parameters
 *      necessary to define the area of the display device within which the
 *      images will be rendered.  The coordinates in this block are given with
 *      respect to the top-left corner of the virtual screen; they do not
 *      necessarily refer to absolute coordinates on the display device.  This
 *      implies that they could refer to window coordinates in a window-based
 *      environment or printer coordinates when a printer is used.
 *
 *      This block is REQUIRED; exactly one Logical Screen Descriptor must be
 *      present per Data Stream.
 *
 *      b. Required Version.  Not applicable. This block is not subject to a
 *      version number. This block must appear immediately after the Header.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Logical Screen Width          Unsigned
 *     +-             -+
 *  1  |               |
 *     +---------------+
 *  2  |               |       Logical Screen Height         Unsigned
 *     +-             -+
 *  3  |               |
 *     +---------------+
 *  4  | |     | |     |       <Packed Fields>               See below
 *     +---------------+
 *  5  |               |       Background Color Index        Byte
 *     +---------------+
 *  6  |               |       Pixel Aspect Ratio            Byte
 *     +---------------+
 *
 *     <Packed Fields>  =      Global Color Table Flag       1 Bit
 *                             Color Resolution              3 Bits
 *                             Sort Flag                     1 Bit
 *                             Size of Global Color Table    3 Bits
 *
 *            i) Logical Screen Width - Width, in pixels, of the Logical Screen
 *            where the images will be rendered in the displaying device.
 *
 *            ii) Logical Screen Height - Height, in pixels, of the Logical
 *            Screen where the images will be rendered in the displaying device.
 *
 *            iii) Global Color Table Flag - Flag indicating the presence of a
 *            Global Color Table; if the flag is set, the Global Color Table will
 *            immediately follow the Logical Screen Descriptor. This flag also
 *            selects the interpretation of the Background Color Index; if the
 *            flag is set, the value of the Background Color Index field should
 *            be used as the table index of the background color. (This field is
 *            the most significant bit of the byte.)
 *
 *            Values :    0 -   No Global Color Table follows, the Background
 *                              Color Index field is meaningless.
 *                        1 -   A Global Color Table will immediately follow, the
 *                              Background Color Index field is meaningful.
 *
 *            iv) Color Resolution - Number of bits per primary color available
 *            to the original image, minus 1. This value represents the size of
 *            the entire palette from which the colors in the graphic were
 *            selected, not the number of colors actually used in the graphic.
 *            For example, if the value in this field is 3, then the palette of
 *            the original image had 4 bits per primary color available to create
 *            the image.  This value should be set to indicate the richness of
 *            the original palette, even if not every color from the whole
 *            palette is available on the source machine.
 *
 *            v) Sort Flag - Indicates whether the Global Color Table is sorted.
 *            If the flag is set, the Global Color Table is sorted, in order of
 *            decreasing importance. Typically, the order would be decreasing
 *            frequency, with most frequent color first. This assists a decoder,
 *            with fewer available colors, in choosing the best subset of colors;
 *            the decoder may use an initial segment of the table to render the
 *            graphic.
 *
 *            Values :    0 -   Not ordered.
 *                        1 -   Ordered by decreasing importance, most
 *                              important color first.
 *
 *            vi) Size of Global Color Table - If the Global Color Table Flag is
 *            set to 1, the value in this field is used to calculate the number
 *            of bytes contained in the Global Color Table. To determine that
 *            actual size of the color table, raise 2 to [the value of the field
 *            + 1].  Even if there is no Global Color Table specified, set this
 *            field according to the above formula so that decoders can choose
 *            the best graphics mode to display the stream in.  (This field is
 *            made up of the 3 least significant bits of the byte.)
 *
 *            vii) Background Color Index - Index into the Global Color Table for
 *
 *            the Background Color. The Background Color is the color used for
 *            those pixels on the screen that are not covered by an image. If the
 *            Global Color Table Flag is set to (zero), this field should be zero
 *            and should be ignored.
 *
 *            viii) Pixel Aspect Ratio - Factor used to compute an approximation
 *            of the aspect ratio of the pixel in the original image.  If the
 *            value of the field is not 0, this approximation of the aspect ratio
 *            is computed based on the formula:
 *
 *            Aspect Ratio = (Pixel Aspect Ratio + 15) / 64
 *
 *            The Pixel Aspect Ratio is defined to be the quotient of the pixel's
 *            width over its height.  The value range in this field allows
 *            specification of the widest pixel of 4:1 to the tallest pixel of
 *            1:4 in increments of 1/64th.
 *
 *            Values :        0 -   No aspect ratio information is given.
 *                       1..255 -   Value used in the computation.
 *
 *      d. Extensions and Scope. The scope of this block is the entire Data
 *      Stream. This block cannot be modified by any extension.
 *
 *      e. Recommendations. None.
 **/
typedef struct 
{
   uint16_t lgc_scr_width;
   uint16_t lgc_scr_height;
   struct global_flag_t
   {
      unsigned char global_color_tbl_sz     : 3;
      unsigned char sort_flag               : 1;
      unsigned char color_resolution        : 3;
      unsigned char global_color_table_flag : 1;
   }  global_flag;
   unsigned char bg_color_index;
   unsigned char pixel_aspect_ratio;
}gif_lgc_scr_desc_t;

/**
 *19. Global Color Table.
 *
 *      a. Description. This block contains a color table, which is a sequence of
 *      bytes representing red-green-blue color triplets. The Global Color Table
 *      is used by images without a Local Color Table and by Plain Text
 *      Extensions. Its presence is marked by the Global Color Table Flag being
 *      set to 1 in the Logical Screen Descriptor; if present, it immediately
 *      follows the Logical Screen Descriptor and contains a number of bytes
 *      equal to
 *                    3 x 2^(Size of Global Color Table+1).
 *
 *      This block is OPTIONAL; at most one Global Color Table may be present
 *      per Data Stream.
 *
 *      b. Required Version.  87a
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +===============+
 *  0  |               |       Red 0                         Byte
 *     +-             -+
 *  1  |               |       Green 0                       Byte
 *     +-             -+
 *  2  |               |       Blue 0                        Byte
 *     +-             -+
 *  3  |               |       Red 1                         Byte
 *     +-             -+
 *     |               |       Green 1                       Byte
 *     +-             -+
 * up  |               |
 *     +-   . . . .   -+       ...
 * to  |               |
 *     +-             -+
 *     |               |       Green 255                     Byte
 *     +-             -+
 *767  |               |       Blue 255                      Byte
 *     +===============+
 *
 *
 *      d. Extensions and Scope. The scope of this block is the entire Data
 *      Stream. This block cannot be modified by any extension.
 *
 *      e. Recommendation. None.
 */
typedef struct 
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
}gif_color_triplet_t;

typedef struct 
{
   gif_color_triplet_t color_table[256];
   size_t size;
}gif_glb_color_tbl_t;

/*
 *20. Image Descriptor.
 *
 *      a. Description. Each image in the Data Stream is composed of an Image
 *      Descriptor, an optional Local Color Table, and the image data.  Each
 *      image must fit within the boundaries of the Logical Screen, as defined
 *      in the Logical Screen Descriptor.
 *
 *      The Image Descriptor contains the parameters necessary to process a table
 *      based image. The coordinates given in this block refer to coordinates
 *      within the Logical Screen, and are given in pixels. This block is a
 *      Graphic-Rendering Block, optionally preceded by one or more Control
 *      blocks such as the Graphic Control Extension, and may be optionally
 *      followed by a Local Color Table; the Image Descriptor is always followed
 *      by the image data.
 *
 *      This block is REQUIRED for an image.  Exactly one Image Descriptor must
 *      be present per image in the Data Stream.  An unlimited number of images
 *      may be present per Data Stream.
 *
 *      b. Required Version.  87a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Image Separator               Byte
 *     +---------------+
 *  1  |               |       Image Left Position           Unsigned
 *     +-             -+
 *  2  |               |
 *     +---------------+
 *  3  |               |       Image Top Position            Unsigned
 *     +-             -+
 *  4  |               |
 *     +---------------+
 *  5  |               |       Image Width                   Unsigned
 *     +-             -+
 *  6  |               |
 *     +---------------+
 *  7  |               |       Image Height                  Unsigned
 *     +-             -+
 *  8  |               |
 *     +---------------+
 *  9  | | | |   |     |       <Packed Fields>               See below
 *     +---------------+
 *
 *     <Packed Fields>  =      Local Color Table Flag        1 Bit
 *                             Interlace Flag                1 Bit
 *                             Sort Flag                     1 Bit
 *                             Reserved                      2 Bits
 *                             Size of Local Color Table     3 Bits
 *
 *           i) Image Separator - Identifies the beginning of an Image
 *           Descriptor. This field contains the fixed value 0x2C.
 *
 *           ii) Image Left Position - Column number, in pixels, of the left edge
 *           of the image, with respect to the left edge of the Logical Screen.
 *           Leftmost column of the Logical Screen is 0.
 *
 *           iii) Image Top Position - Row number, in pixels, of the top edge of
 *           the image with respect to the top edge of the Logical Screen. Top
 *           row of the Logical Screen is 0.
 *
 *           iv) Image Width - Width of the image in pixels.
 *
 *           v) Image Height - Height of the image in pixels.
 *
 *           vi) Local Color Table Flag - Indicates the presence of a Local Color
 *           Table immediately following this Image Descriptor. (This field is
 *           the most significant bit of the byte.)
 *
 *
 *           Values :    0 -   Local Color Table is not present. Use
 *                             Global Color Table if available.
 *                       1 -   Local Color Table present, and to follow
 *                             immediately after this Image Descriptor.
 *
 *           vii) Interlace Flag - Indicates if the image is interlaced. An image
 *           is interlaced in a four-pass interlace pattern; see Appendix E for
 *           details.
 *
 *           Values :    0 - Image is not interlaced.
 *                       1 - Image is interlaced.
 *
 *            viii) Sort Flag - Indicates whether the Local Color Table is
 *            sorted.  If the flag is set, the Local Color Table is sorted, in
 *            order of decreasing importance. Typically, the order would be
 *            decreasing frequency, with most frequent color first. This assists
 *            a decoder, with fewer available colors, in choosing the best subset
 *            of colors; the decoder may use an initial segment of the table to
 *            render the graphic.
 *
 *            Values :    0 -   Not ordered.
 *                        1 -   Ordered by decreasing importance, most
 *                              important color first.
 *
 *            ix) Size of Local Color Table - If the Local Color Table Flag is
 *            set to 1, the value in this field is used to calculate the number
 *            of bytes contained in the Local Color Table. To determine that
 *            actual size of the color table, raise 2 to the value of the field
 *            + 1. This value should be 0 if there is no Local Color Table
 *            specified. (This field is made up of the 3 least significant bits
 *            of the byte.)
 *
 *     d. Extensions and Scope. The scope of this block is the Table-based Image
 *     Data Block that follows it. This block may be modified by the Graphic
 *     Control Extension.
 *
 *     e. Recommendation. None.
 */

typedef struct 
{
   unsigned char image_sep;
   uint16_t image_left;
   uint16_t image_top;
   uint16_t image_width;
   uint16_t image_height;
   struct local_flag_t
   {
      unsigned char local_color_tbl_sz   : 3;
      unsigned char reserved             : 2;
      unsigned char sort_flag            : 1;
      unsigned char interlace_flag       : 1;
      unsigned char local_color_tbl_flag : 1;
   }  local_flag;
}gif_image_desc_t;

/**
 *21. Local Color Table.
 *
 *     a. Description. This block contains a color table, which is a sequence of
 *     bytes representing red-green-blue color triplets. The Local Color Table
 *     is used by the image that immediately follows. Its presence is marked by
 *     the Local Color Table Flag being set to 1 in the Image Descriptor; if
 *     present, the Local Color Table immediately follows the Image Descriptor
 *     and contains a number of bytes equal to
 *                          3x2^(Size of Local Color Table+1).
 *     If present, this color table temporarily becomes the active color table
 *     and the following image should be processed using it. This block is
 *     OPTIONAL; at most one Local Color Table may be present per Image
 *     Descriptor and its scope is the single image associated with the Image
 *     Descriptor that precedes it.
 *
 *     b. Required Version.  87a.
 *
 *     c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +===============+
 *  0  |               |       Red 0                         Byte
 *     +-             -+
 *  1  |               |       Green 0                       Byte
 *     +-             -+
 *  2  |               |       Blue 0                        Byte
 *     +-             -+
 *  3  |               |       Red 1                         Byte
 *     +-             -+
 *     |               |       Green 1                       Byte
 *     +-             -+
 * up  |               |
 *     +-   . . . .   -+       ...
 * to  |               |
 *     +-             -+
 *     |               |       Green 255                     Byte
 *     +-             -+
 *767  |               |       Blue 255                      Byte
 *     +===============+
 *
 *
 *     d. Extensions and Scope. The scope of this block is the Table-based Image
 *     Data Block that immediately follows it. This block cannot be modified by
 *     any extension.
 *
 *     e. Recommendations. None.
 */
typedef struct 
{
   gif_color_triplet_t color_table[256];
   size_t size;
}gif_lcl_color_tbl_t;

/**
 *22. Table Based Image Data.
 *
 *     a. Description. The image data for a table based image consists of a
 *     sequence of sub-blocks, of size at most 255 bytes each, containing an
 *     index into the active color table, for each pixel in the image.  Pixel
 *     indices are in order of left to right and from top to bottom.  Each index
 *     must be within the range of the size of the active color table, starting
 *     at 0. The sequence of indices is encoded using the LZW Algorithm with
 *     variable-length code, as described in Appendix F
 *
 *     b. Required Version.  87a.
 *
 *     c. Syntax. The image data format is as follows:
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *     |               |       LZW Minimum Code Size         Byte
 *     +---------------+
 *
 *     +===============+
 *     |               |
 *     /               /       Image Data                    Data Sub-blocks
 *     |               |
 *     +===============+
 *
 *            i) LZW Minimum Code Size.  This byte determines the initial number
 *            of bits used for LZW codes in the image data, as described in
 *            Appendix F.
 *
 *     d. Extensions and Scope. This block has no scope, it contains raster
 *     data. Extensions intended to modify a Table-based image must appear
 *     before the corresponding Image Descriptor.
 *
 *     e. Recommendations. None.
 */

typedef struct
{
   unsigned char lzw_code_size;
}gif_lzw_code_size_t;

typedef struct 
{
   unsigned char block_size;
   char data_value[256];
}gif_image_data_block_t;

typedef struct
{
   unsigned char terminator;
}gif_image_data_ter_t;

typedef struct 
{
   unsigned char lzw_min_code_size;
   unsigned char *data;
   size_t data_len;
}gif_table_based_image_data_t;

/*
 *23. Graphic Control Extension.
 *
 *      a. Description. The Graphic Control Extension contains parameters used
 *      when processing a graphic rendering block. The scope of this extension is
 *      the first graphic rendering block to follow. The extension contains only
 *      one data sub-block.
 *
 *      This block is OPTIONAL; at most one Graphic Control Extension may precede
 *      a graphic rendering block. This is the only limit to the number of
 *      Graphic Control Extensions that may be contained in a Data Stream.
 *
 *      b. Required Version.  89a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Extension Introducer          Byte
 *     +---------------+
 *  1  |               |       Graphic Control Label         Byte
 *     +---------------+
 *
 *     +---------------+
 *  0  |               |       Block Size                    Byte
 *     +---------------+
 *  1  |     |     | | |       <Packed Fields>               See below
 *     +---------------+
 *  2  |               |       Delay Time                    Unsigned
 *     +-             -+
 *  3  |               |
 *     +---------------+
 *  4  |               |       Transparent Color Index       Byte
 *     +---------------+
 *
 *     +---------------+
 *  0  |               |       Block Terminator              Byte
 *     +---------------+
 *
 *
 *      <Packed Fields>  =     Reserved                      3 Bits
 *                             Disposal Method               3 Bits
 *                             User Input Flag               1 Bit
 *                             Transparent Color Flag        1 Bit
 *
 *            i) Extension Introducer - Identifies the beginning of an extension
 *            block. This field contains the fixed value 0x21.
 *
 *            ii) Graphic Control Label - Identifies the current block as a
 *            Graphic Control Extension. This field contains the fixed value
 *            0xF9.
 *
 *            iii) Block Size - Number of bytes in the block, after the Block
 *            Size field and up to but not including the Block Terminator.  This
 *            field contains the fixed value 4.
 *
 *            iv) Disposal Method - Indicates the way in which the graphic is to
 *            be treated after being displayed.
 *
 *            Values :    0 -   No disposal specified. The decoder is
 *                              not required to take any action.
 *                        1 -   Do not dispose. The graphic is to be left
 *                              in place.
 *                        2 -   Restore to background color. The area used by the
 *                              graphic must be restored to the background color.
 *                        3 -   Restore to previous. The decoder is required to
 *                              restore the area overwritten by the graphic with
 *                              what was there prior to rendering the graphic.
 *                     4-7 -    To be defined.
 *
 *            v) User Input Flag - Indicates whether or not user input is
 *            expected before continuing. If the flag is set, processing will
 *            continue when user input is entered. The nature of the User input
 *            is determined by the application (Carriage Return, Mouse Button
 *            Click, etc.).
 *
 *            Values :    0 -   User input is not expected.
 *                        1 -   User input is expected.
 *
 *            When a Delay Time is used and the User Input Flag is set,
 *            processing will continue when user input is received or when the
 *            delay time expires, whichever occurs first.
 *
 *            vi) Transparency Flag - Indicates whether a transparency index is
 *            given in the Transparent Index field. (This field is the least
 *            significant bit of the byte.)
 *
 *            Values :    0 -   Transparent Index is not given.
 *                        1 -   Transparent Index is given.
 *
 *            vii) Delay Time - If not 0, this field specifies the number of
 *            hundredths (1/100) of a second to wait before continuing with the
 *            processing of the Data Stream. The clock starts ticking immediately
 *            after the graphic is rendered. This field may be used in
 *            conjunction with the User Input Flag field.
 *
 *            viii) Transparency Index - The Transparency Index is such that when
 *            encountered, the corresponding pixel of the display device is not
 *            modified and processing goes on to the next pixel. The index is
 *            present if and only if the Transparency Flag is set to 1.
 *
 *            ix) Block Terminator - This zero-length data block marks the end of
 *            the Graphic Control Extension.
 *
 *      d. Extensions and Scope. The scope of this Extension is the graphic
 *      rendering block that follows it; it is possible for other extensions to
 *      be present between this block and its target. This block can modify the
 *      Image Descriptor Block and the Plain Text Extension.
 *
 *      e. Recommendations.
 *
 *            i) Disposal Method - The mode Restore To Previous is intended to be
 *            used in small sections of the graphic; the use of this mode imposes
 *            severe demands on the decoder to store the section of the graphic
 *            that needs to be saved. For this reason, this mode should be used
 *            sparingly.  This mode is not intended to save an entire graphic or
 *            large areas of a graphic; when this is the case, the encoder should
 *            make every attempt to make the sections of the graphic to be
 *            restored be separate graphics in the data stream. In the case where
 *            a decoder is not capable of saving an area of a graphic marked as
 *            Restore To Previous, it is recommended that a decoder restore to
 *            the background color.
 *
 *            ii) User Input Flag - When the flag is set, indicating that user
 *            input is expected, the decoder may sound the bell (0x07) to alert
 *            the user that input is being expected.  In the absence of a
 *            specified Delay Time, the decoder should wait for user input
 *            indefinitely.  It is recommended that the encoder not set the User
 *            Input Flag without a Delay Time specified.
 */
typedef struct 
{
   unsigned char ext_introducer;
   unsigned char graphic_ctrl_label;

   unsigned char block_size;
   struct flag_t
   {
      unsigned char transparent_color_flag   : 1;
      unsigned char user_input_flag          : 1;
      unsigned char disposal_method          : 3;
      unsigned char reserved                 : 3;
   }  flag;
   uint16_t delay_time;
   unsigned char transparent_color_index;
   unsigned char block_terminator;
}gif_graphic_ctrl_ext_t;

/**
 *24. Comment Extension.
 *
 *      a. Description. The Comment Extension contains textual information which
 *      is not part of the actual graphics in the GIF Data Stream. It is suitable
 *      for including comments about the graphics, credits, descriptions or any
 *      other type of non-control and non-graphic data.  The Comment Extension
 *      may be ignored by the decoder, or it may be saved for later processing;
 *      under no circumstances should a Comment Extension disrupt or interfere
 *      with the processing of the Data Stream.
 *
 *      This block is OPTIONAL; any number of them may appear in the Data Stream.
 *
 *      b. Required Version.  89a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Extension Introducer          Byte
 *     +---------------+
 *  1  |               |       Comment Label                 Byte
 *     +---------------+
 *
 *     +===============+
 *     |               |
 *  N  |               |       Comment Data                  Data Sub-blocks
 *     |               |
 *     +===============+
 *
 *     +---------------+
 *  0  |               |       Block Terminator              Byte
 *     +---------------+
 *
 *            i) Extension Introducer - Identifies the beginning of an extension
 *            block. This field contains the fixed value 0x21.
 *
 *            ii) Comment Label - Identifies the block as a Comment Extension.
 *            This field contains the fixed value 0xFE.
 *
 *            iii) Comment Data - Sequence of sub-blocks, each of size at most
 *            255 bytes and at least 1 byte, with the size in a byte preceding
 *            the data.  The end of the sequence is marked by the Block
 *            Terminator.
 *
 *            iv) Block Terminator - This zero-length data block marks the end of
 *            the Comment Extension.
 *
 *      d. Extensions and Scope. This block does not have scope. This block
 *      cannot be modified by any extension.
 *
 *      e. Recommendations.
 *
 *            i) Data - This block is intended for humans.  It should contain
 *            text using the 7-bit ASCII character set. This block should
 *            not be used to store control information for custom processing.
 *
 *            ii) Position - This block may appear at any point in the Data
 *            Stream at which a block can begin; however, it is recommended that
 *            Comment Extensions do not interfere with Control or Data blocks;
 *            they should be located at the beginning or at the end of the Data
 *            Stream to the extent possible.
 */
typedef struct 
{
   unsigned char ext_introducer;
   unsigned char comment_label;
}gif_comment_ext_t;

/**
 *25. Plain Text Extension.
 *
 *      a. Description. The Plain Text Extension contains textual data and the
 *      parameters necessary to render that data as a graphic, in a simple form.
 *      The textual data will be encoded with the 7-bit printable ASCII
 *      characters.  Text data are rendered using a grid of character cells
 *      defined by the parameters in the block fields. Each character is rendered
 *      in an individual cell. The textual data in this block is to be rendered
 *      as mono-spaced characters, one character per cell, with a best fitting
 *      font and size. For further information, see the section on
 *      Recommendations below. The data characters are taken sequentially from
 *      the data portion of the block and rendered within a cell, starting with
 *      the upper left cell in the grid and proceeding from left to right and
 *      from top to bottom. Text data is rendered until the end of data is
 *      reached or the character grid is filled.  The Character Grid contains an
 *      integral number of cells; in the case that the cell dimensions do not
 *      allow for an integral number, fractional cells must be discarded; an
 *      encoder must be careful to specify the grid dimensions accurately so that
 *      this does not happen. This block requires a Global Color Table to be
 *      available; the colors used by this block reference the Global Color Table
 *      in the Stream if there is one, or the Global Color Table from a previous
 *      Stream, if one was saved. This block is a graphic rendering block,
 *      therefore it may be modified by a Graphic Control Extension.  This block
 *      is OPTIONAL; any number of them may appear in the Data Stream.
 *
 *      b. Required Version.  89a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Extension Introducer          Byte
 *     +---------------+
 *  1  |               |       Plain Text Label              Byte
 *     +---------------+
 *
 *     +---------------+
 *  0  |               |       Block Size                    Byte
 *     +---------------+
 *  1  |               |       Text Grid Left Position       Unsigned
 *     +-             -+
 *  2  |               |
 *     +---------------+
 *  3  |               |       Text Grid Top Position        Unsigned
 *     +-             -+
 *  4  |               |
 *     +---------------+
 *  5  |               |       Text Grid Width               Unsigned
 *     +-             -+
 *  6  |               |
 *     +---------------+
 *  7  |               |       Text Grid Height              Unsigned
 *     +-             -+
 *  8  |               |
 *     +---------------+
 *  9  |               |       Character Cell Width          Byte
 *     +---------------+
 * 10  |               |       Character Cell Height         Byte
 *     +---------------+
 * 11  |               |       Text Foreground Color Index   Byte
 *     +---------------+
 * 12  |               |       Text Background Color Index   Byte
 *     +---------------+
 *
 *     +===============+
 *     |               |
 *  N  |               |       Plain Text Data               Data Sub-blocks
 *     |               |
 *     +===============+
 *
 *     +---------------+
 *  0  |               |       Block Terminator              Byte
 *     +---------------+
 *
 *            i) Extension Introducer - Identifies the beginning of an extension
 *            block. This field contains the fixed value 0x21.
 *
 *            ii) Plain Text Label - Identifies the current block as a Plain Text
 *            Extension. This field contains the fixed value 0x01.
 *
 *            iii) Block Size - Number of bytes in the extension, after the Block
 *            Size field and up to but not including the beginning of the data
 *            portion. This field contains the fixed value 12.
 *
 *            iv) Text Grid Left Position - Column number, in pixels, of the left
 *            edge of the text grid, with respect to the left edge of the Logical
 *            Screen.
 *
 *            v) Text Grid Top Position - Row number, in pixels, of the top edge
 *            of the text grid, with respect to the top edge of the Logical
 *            Screen.
 *
 *            vi) Image Grid Width - Width of the text grid in pixels.
 *
 *            vii) Image Grid Height - Height of the text grid in pixels.
 *
 *            viii) Character Cell Width - Width, in pixels, of each cell in the
 *            grid.
 *
 *            ix) Character Cell Height - Height, in pixels, of each cell in the
 *            grid.
 *
 *            x) Text Foreground Color Index - Index into the Global Color Table
 *            to be used to render the text foreground.
 *
 *            xi) Text Background Color Index - Index into the Global Color Table
 *            to be used to render the text background.
 *
 *            xii) Plain Text Data - Sequence of sub-blocks, each of size at most
 *            255 bytes and at least 1 byte, with the size in a byte preceding
 *            the data.  The end of the sequence is marked by the Block
 *            Terminator.
 *
 *            xiii) Block Terminator - This zero-length data block marks the end
 *            of the Plain Text Data Blocks.
 *
 *      d. Extensions and Scope. The scope of this block is the Plain Text Data
 *      Block contained in it. This block may be modified by the Graphic Control
 *      Extension.
 *
 *      e. Recommendations. The data in the Plain Text Extension is assumed to be
 *      preformatted. The selection of font and size is left to the discretion of
 *      the decoder.  If characters less than 0x20 or greater than 0xf7 are
 *      encountered, it is recommended that the decoder display a Space character
 *      (0x20). The encoder should use grid and cell dimensions such that an
 *      integral number of cells fit in the grid both horizontally as well as
 *      vertically.  For broadest compatibility, character cell dimensions should
 *      be around 8x8 or 8x16 (width x height); consider an image for unusual
 *      sized text.
 */
typedef struct 
{
   unsigned char ext_introducer;
   unsigned char plain_text_lable;

   unsigned char block_size;
   uint16_t text_grid_left;
   uint16_t text_grid_top;
   uint16_t text_grid_width;
   uint16_t text_grid_height;
   unsigned char char_cell_width;
   unsigned char char_cell_height;
   unsigned char fore_color_index;
   unsigned char back_color_index;
}gif_plain_text_ext_t;

/**
 *26. Application Extension.
 *
 *      a. Description. The Application Extension contains application-specific
 *      information; it conforms with the extension block syntax, as described
 *      below, and its block label is 0xFF.
 *
 *      b. Required Version.  89a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       Extension Introducer          Byte
 *     +---------------+
 *  1  |               |       Extension Label               Byte
 *     +---------------+
 *
 *     +---------------+
 *  0  |               |       Block Size                    Byte
 *     +---------------+
 *  1  |               |
 *     +-             -+
 *  2  |               |
 *     +-             -+
 *  3  |               |       Application Identifier        8 Bytes
 *     +-             -+
 *  4  |               |
 *     +-             -+
 *  5  |               |
 *     +-             -+
 *  6  |               |
 *     +-             -+
 *  7  |               |
 *     +-             -+
 *  8  |               |
 *     +---------------+
 *  9  |               |
 *     +-             -+
 * 10  |               |       Appl. Authentication Code     3 Bytes
 *     +-             -+
 * 11  |               |
 *     +---------------+
 *
 *     +===============+
 *     |               |
 *     |               |       Application Data              Data Sub-blocks
 *     |               |
 *     |               |
 *     +===============+
 *
 *     +---------------+
 *  0  |               |       Block Terminator              Byte
 *     +---------------+
 *
 *            i) Extension Introducer - Defines this block as an extension. This
 *            field contains the fixed value 0x21.
 *
 *            ii) Application Extension Label - Identifies the block as an
 *            Application Extension. This field contains the fixed value 0xFF.
 *
 *            iii) Block Size - Number of bytes in this extension block,
 *            following the Block Size field, up to but not including the
 *            beginning of the Application Data. This field contains the fixed
 *            value 11.
 *
 *            iv) Application Identifier - Sequence of eight printable ASCII
 *            characters used to identify the application owning the Application
 *            Extension.
 *
 *            v) Application Authentication Code - Sequence of three bytes used
 *            to authenticate the Application Identifier. An Application program
 *            may use an algorithm to compute a binary code that uniquely
 *            identifies it as the application owning the Application Extension.
 *
 *
 *      d. Extensions and Scope. This block does not have scope. This block
 *      cannot be modified by any extension.
 *
 *      e. Recommendation. None.
 */
typedef struct 
{
   unsigned char ext_introducer;
   unsigned char plain_text_label;

   unsigned char block_size;
   char identifier[8];
   char appl_auth_code[3];
}gif_appl_ext_t;

/**
 *27. Trailer.
 *
 *      a. Description. This block is a single-field block indicating the end of
 *      the GIF Data Stream.  It contains the fixed value 0x3B.
 *
 *      b. Required Version.  87a.
 *
 *      c. Syntax.
 *
 *      7 6 5 4 3 2 1 0        Field Name                    Type
 *     +---------------+
 *  0  |               |       GIF Trailer                   Byte
 *     +---------------+
 *
 *      d. Extensions and Scope. This block does not have scope, it terminates
 *      the GIF Data Stream. This block may not be modified by any extension.
 *
 *      e. recommendations. none.
 */
typedef struct
{
   unsigned char trailer;
}gif_trailer_t;


#pragma   pack()
#endif /*GIF_H*/

