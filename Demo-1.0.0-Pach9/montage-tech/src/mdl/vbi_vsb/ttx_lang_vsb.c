/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"

#include "ttx_lang_vsb.h"
#include "ttx_dec_vsb.h"
#include "ttx_format_vsb.h"

/*
 *  Teletext character set
 */

/*
 *  ETS 300 706 Table 36: Latin National Option Sub-sets
 *
 *  Latin G0 character code to Unicode mapping per national subset,
 *  unmodified codes (CHAR_SUBSET_NONE) in row zero.
 *
 *  [13][0] Turkish currency symbol not in Unicode, use private code U+E800
 */
static const u16 national_subset_vsb[14][13] =
{
    {
        0x0023u, 0x0024u, 0x0040u, 0x005Bu, 0x005Cu, 0x005Du, 0x005Eu,
        0x005Fu, 0x0060u, 0x007Bu, 0x007Cu, 0x007Du, 0x007Eu
    },
    {
        0x0023u, 0x016Fu, 0x010Du, 0x0165u, 0x017Eu, 0x00FDu, 0x00EDu,
        0x0159u, 0x00E9u, 0x00E1u, 0x011Bu, 0x00FAu, 0x0161u
    },
    {
        0x00A3u, 0x0024u, 0x0040u, 0x2190u, 0x00BDu, 0x2192u, 0x2191u,
        0x0023u, 0x2014u, 0x00BCu, 0x2016u, 0x00BEu, 0x00F7u
    },
    {
        0x0023u, 0x00F5u, 0x0160u, 0x00C4u, 0x00D6u, 0x017Du, 0x00DCu,
        0x00D5u, 0x0161u, 0x00E4u, 0x00F6u, 0x017Eu, 0x00FCu
    },
    {
        0x00E9u, 0x00EFu, 0x00E0u, 0x00EBu, 0x00EAu, 0x00F9u, 0x00EEu,
        0x0023u, 0x00E8u, 0x00E2u, 0x00F4u, 0x00FBu, 0x00E7u
    },
    {
        0x0023u, 0x0024u, 0x00A7u, 0x00C4u, 0x00D6u, 0x00DCu, 0x005Eu,
        0x005Fu, 0x00B0u, 0x00E4u, 0x00F6u, 0x00FCu, 0x00DFu
    },
    {
        0x00A3u, 0x0024u, 0x00E9u, 0x00B0u, 0x00E7u, 0x2192u, 0x2191u,
        0x0023u, 0x00F9u, 0x00E0u, 0x00F2u, 0x00E8u, 0x00ECu
    },
    {
        0x0023u, 0x0024u, 0x0160u, 0x0117u, 0x0229u, 0x017Du, 0x010Du,
        0x016Bu, 0x0161u, 0x0105u, 0x0173u, 0x017Eu, 0x012Fu
    },
    {
        0x0023u, 0x0144u, 0x0105u, 0x01B5u, 0x015Au, 0x0141u, 0x0107u,
        0x00F3u, 0x0119u, 0x017Cu, 0x015Bu, 0x0142u, 0x017Au
    },
    {
        0x00E7u, 0x0024u, 0x00A1u, 0x00E1u, 0x00E9u, 0x00EDu, 0x00F3u,
        0x00FAu, 0x00BFu, 0x00FCu, 0x00F1u, 0x00E8u, 0x00E0u
    },
    {
        0x0023u, 0x00A4u, 0x0162u, 0x00C2u, 0x015Eu, 0x01CDu, 0x00CDu,
        0x0131u, 0x0163u, 0x00E2u, 0x015Fu, 0X01CEu, 0x00EEu
    },
    {
        0x0023u, 0x00CBu, 0x010Cu, 0x0106u, 0x017Du, 0x00D0u, 0x0160u,
        0x00EBu, 0x010Du, 0x0107u, 0x017Eu, 0x00F0u, 0x0161u
    },
    {
        0x0023u, 0x00A4u, 0x00C9u, 0x00C4u, 0x00D6u, 0x00C5u, 0x00DCu,
        0x005Fu, 0x00E9u, 0x00E4u, 0x00F6u, 0x00E5u, 0x00FCu
    },
    {
        0xE800u, 0x011Fu, 0x0130u, 0x015Eu, 0x00D6u, 0x00C7u, 0x00DCu,
        0x011Eu, 0x0131u, 0x015Fu, 0x00F6u, 0x00E7u, 0x00FCu
    }
};

/*
 *  ETS 300 706 Table 37: Latin G2 Supplementary Set
 *
 *  0x49 seems to be dot below; not in Unicode (except combining), use U+002E.
 */
static const u16 latin_g2_vsb[96] =
{
  0x00A0u, 0x00A1u, 0x00A2u, 0x00A3u, 0x0024u, 0x00A5u, 0x0023u, 0x00A7u,
  0x00A4u, 0x2018u, 0x201Cu, 0x00ABu, 0x2190u, 0x2191u, 0x2192u, 0x2193u,
  0x00B0u, 0x00B1u, 0x00B2u, 0x00B3u, 0x00D7u, 0x00B5u, 0x00B6u, 0x00B7u,
  0x00F7u, 0x2019u, 0x201Du, 0x00BBu, 0x00BCu, 0x00BDu, 0x00BEu, 0x00BFu,
  0x0020u, 0x02CBu, 0x02CAu, 0x02C6u, 0x02DCu, 0x02C9u, 0x02D8u, 0x02D9u,
  0x00A8u, 0x002Eu, 0x02DAu, 0x02CFu, 0x02CDu, 0x02DDu, 0x02DBu, 0x02C7u,
  0x2014u, 0x00B9u, 0x00AEu, 0x00A9u, 0x2122u, 0x266Au, 0x20A0u, 0x2030u,
  0x0251u, 0x0020u, 0x0020u, 0x0020u, 0x215Bu, 0x215Cu, 0x215Du, 0x215Eu,
  0x2126u, 0x00C6u, 0x00D0u, 0x00AAu, 0x0126u, 0x0020u, 0x0132u, 0x013Fu,
  0x0141u, 0x00D8u, 0x0152u, 0x00BAu, 0x00DEu, 0x0166u, 0x014Au, 0x0149u,
  0x0138u, 0x00E6u, 0x0111u, 0x00F0u, 0x0127u, 0x0131u, 0x0133u, 0x0140u,
  0x0142u, 0x00F8u, 0x0153u, 0x00DFu, 0x00FEu, 0x0167u, 0x014Bu, 0x25A0u
};

/*
 *  ETS 300 706 Table 38: Cyrillic G0 Primary Set - Option 1 - Serbian/Croatian
 */
static const u16 cyrillic_1_g0_vsb[64] =
{
  0x0427u, 0x0410u, 0x0411u, 0x0426u, 0x0414u, 0x0415u, 0x0424u, 0x0413u,
  0x0425u, 0x0418u, 0x0408u, 0x041Au, 0x041Bu, 0x041Cu, 0x041Du, 0x041Eu,
  0x041Fu, 0x040Cu, 0x0420u, 0x0421u, 0x0422u, 0x0423u, 0x0412u, 0x0403u,
  0x0409u, 0x040Au, 0x0417u, 0x040Bu, 0x0416u, 0x0402u, 0x0428u, 0x040Fu,
  0x0447u, 0x0430u, 0x0431u, 0x0446u, 0x0434u, 0x0435u, 0x0444u, 0x0433u,
  0x0445u, 0x0438u, 0x0458u, 0x043Au, 0x043Bu, 0x043Cu, 0x043Du, 0x043Eu,
  0x043Fu, 0x045Cu, 0x0440u, 0x0441u, 0x0442u, 0x0443u, 0x0432u, 0x0453u,
  0x0459u, 0x045Au, 0x0437u, 0x045Bu, 0x0436u, 0x0452u, 0x0448u, 0x25A0u
};

/*
 *  ETS 300 706 Table 39: Cyrillic G0 Primary Set - Option 2 - Russian/Bulgarian
 */
static const u16 cyrillic_2_g0_vsb[64] =
{
  0x042Eu, 0x0410u, 0x0411u, 0x0426u, 0x0414u, 0x0415u, 0x0424u, 0x0413u,
  0x0425u, 0x0418u, 0x040Du, 0x041Au, 0x041Bu, 0x041Cu, 0x041Du, 0x041Eu,
  0x041Fu, 0x042Fu, 0x0420u, 0x0421u, 0x0422u, 0x0423u, 0x0416u, 0x0412u,
  0x042Cu, 0x042Au, 0x0417u, 0x0428u, 0x042Du, 0x0429u, 0x0427u, 0x042Bu,
  0x044Eu, 0x0430u, 0x0431u, 0x0446u, 0x0434u, 0x0435u, 0x0444u, 0x0433u,
  0x0445u, 0x0438u, 0x045Du, 0x043Au, 0x043Bu, 0x043Cu, 0x043Du, 0x043Eu,
  0x043Fu, 0x044Fu, 0x0440u, 0x0441u, 0x0442u, 0x0443u, 0x0436u, 0x0432u,
  0x044Cu, 0x044Au, 0x0437u, 0x0448u, 0x044Du, 0x0449u, 0x0447u, 0x25A0u
};

/*
 *  ETS 300 706 Table 40: Cyrillic G0 Primary Set - Option 3 - Ukrainian 
 */
static const u16 cyrillic_3_g0[64] =
{
  0x042Eu, 0x0410u, 0x0411u, 0x0426u, 0x0414u, 0x0415u, 0x0424u, 0x0413u,
  0x0425u, 0x0418u, 0x040Du, 0x041Au, 0x041Bu, 0x041Cu, 0x041Du, 0x041Eu,
  0x041Fu, 0x042Fu, 0x0420u, 0x0421u, 0x0422u, 0x0423u, 0x0416u, 0x0412u,
  0x042Cu, 0x0406u, 0x0417u, 0x0428u, 0x0404u, 0x0429u, 0x0427u, 0x0407u,
  0x044Eu, 0x0430u, 0x0431u, 0x0446u, 0x0434u, 0x0435u, 0x0444u, 0x0433u,
  0x0445u, 0x0438u, 0x045Du, 0x043Au, 0x043Bu, 0x043Cu, 0x043Du, 0x043Eu,
  0x043Fu, 0x044Fu, 0x0440u, 0x0441u, 0x0442u, 0x0443u, 0x0436u, 0x0432u,
  0x044Cu, 0x0456u, 0x0437u, 0x0448u, 0x0454u, 0x0449u, 0x0447u, 0x25A0u
};

/*
 *  ETS 300 706 Table 41: Cyrillic G2 Supplementary Set
 */
static const u16 cyrillic_g2_vsb[96] =
{
  0x00A0u, 0x00A1u, 0x00A2u, 0x00A3u, 0x0020u, 0x00A5u, 0x0023u, 0x00A7u,
  0x0020u, 0x2018u, 0x201Cu, 0x00ABu, 0x2190u, 0x2191u, 0x2192u, 0x2193u,
  0x00B0u, 0x00B1u, 0x00B2u, 0x00B3u, 0x00D7u, 0x00B5u, 0x00B6u, 0x00B7u,
  0x00F7u, 0x2019u, 0x201Du, 0x00BBu, 0x00BCu, 0x00BDu, 0x00BEu, 0x00BFu,
  0x0020u, 0x02CBu, 0x02CAu, 0x02C6u, 0x02DCu, 0x02C9u, 0x02D8u, 0x02D9u,
  0x00A8u, 0x002Eu, 0x02DAu, 0x02CFu, 0x02CDu, 0x02DDu, 0x02DBu, 0x02C7u,
  0x2014u, 0x00B9u, 0x00AEu, 0x00A9u, 0x2122u, 0x266Au, 0x20A0u, 0x2030u,
  0x0251u, 0x0141u, 0x0142u, 0x00DFu, 0x215Bu, 0x215Cu, 0x215Du, 0x215Eu,
  0x0044u, 0x0045u, 0x0046u, 0x0047u, 0x0049u, 0x004Au, 0x004Bu, 0x004Cu,
  0x004Eu, 0x0051u, 0x0052u, 0x0053u, 0x0055u, 0x0056u, 0x0057u, 0x005Au,
  0x0064u, 0x0065u, 0x0066u, 0x0067u, 0x0069u, 0x006Au, 0x006Bu, 0x006Cu,
  0x006Eu, 0x0071u, 0x0072u, 0x0073u, 0x0075u, 0x0076u, 0x0077u, 0x007Au
};

/*
 *  ETS 300 706 Table 42: Greek G0 Primary Set
 */
static const u16 greek_g0_vsb[64] =
{
  0x0390u, 0x0391u, 0x0392u, 0x0393u, 0x0394u, 0x0395u, 0x0396u, 0x0397u,
  0x0398u, 0x0399u, 0x039Au, 0x039Bu, 0x039Cu, 0x039Du, 0x039Eu, 0x039Fu,
  0x03A0u, 0x03A1u, 0x0374u, 0x03A3u, 0x03A4u, 0x03A5u, 0x03A6u, 0x03A7u,
  0x03A8u, 0x03A9u, 0x03AAu, 0x03ABu, 0x03ACu, 0x03ADu, 0x03AEu, 0x03AFu,
  0x03B0u, 0x03B1u, 0x03B2u, 0x03B3u, 0x03B4u, 0x03B5u, 0x03B6u, 0x03B7u,
  0x03B8u, 0x03B9u, 0x03BAu, 0x03BBu, 0x03BCu, 0x03BDu, 0x03BEu, 0x03BFu,
  0x03C0u, 0x03C1u, 0x03C2u, 0x03C3u, 0x03C4u, 0x03C5u, 0x03C6u, 0x03C7u,
  0x03C8u, 0x03C9u, 0x03CAu, 0x03CBu, 0x03CCu, 0x03CDu, 0x03CEu, 0x25A0u
};

/*
 *  ETS 300 706 Table 43: Greek G2 Supplementary Set
 */
static const u16 greek_g2_vsb[96] =
{
  0x00A0u, 0x0061u, 0x0062u, 0x00A3u, 0x0065u, 0x0068u, 0x0069u, 0x00A7u,
  0x003Au, 0x2018u, 0x201Cu, 0x006Bu, 0x2190u, 0x2191u, 0x2192u, 0x2193u,
  0x00B0u, 0x00B1u, 0x00B2u, 0x00B3u, 0x0078u, 0x006Du, 0x006Eu, 0x0070u,
  0x00F7u, 0x2019u, 0x201Du, 0x0074u, 0x00BCu, 0x00BDu, 0x00BEu, 0x0078u,
  0x0020u, 0x02CBu, 0x02CAu, 0x02C6u, 0x02DCu, 0x02C9u, 0x02D8u, 0x02D9u,
  0x00A8u, 0x002Eu, 0x02DAu, 0x02CFu, 0x02CDu, 0x02DDu, 0x02DBu, 0x02C7u,
  0x003Fu, 0x00B9u, 0x00AEu, 0x00A9u, 0x2122u, 0x266Au, 0x20A0u, 0x2030u,
  0x0251u, 0x038Au, 0x038Eu, 0x038Fu, 0x215Bu, 0x215Cu, 0x215Du, 0x215Eu,
  0x0043u, 0x0044u, 0x0046u, 0x0047u, 0x004Au, 0x004Cu, 0x0051u, 0x0052u,
  0x0053u, 0x0055u, 0x0056u, 0x0057u, 0x0059u, 0x005Au, 0x0386u, 0x0389u,
  0x0063u, 0x0064u, 0x0066u, 0x0067u, 0x006Au, 0x006Cu, 0x0071u, 0x0072u,
  0x0073u, 0x0075u, 0x0076u, 0x0077u, 0x0079u, 0x007Au, 0x0388u, 0x25A0u
};

/*
 *  ETS 300 706 Table 44: Arabic G0 Primary Set
 *
 *  XXX 0X0000 is what?
 *  Until these tables are finished use private codes U+E6xx.
 */
static const u16 arabic_g0_vsb[96] =
{
/*
  0x0020, 0x0021, 0x0022, 0x00A3, 0x0024, 0x0025, 0x0000, 0x0000,
  0x0029, 0x0028, 0x002A, 0x002B, 0x060C, 0x002D, 0x002E, 0x002F,
  0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
  0x0038, 0x0039, 0x003A, 0x061B, 0x003E, 0x003D, 0x003C, 0x061F,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,
  0x0638, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0023,
  0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647,
  0x0648, 0x0649, 0x064A, 0x062B, 0x062D, 0x062C, 0x062E, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x25A0
*/
  0x0020u, 0x0021u, 0x0022u, 0x00A3u, 0x0024u, 0x0025u, 0xE606u, 0xE607u,
  0x0029u, 0x0028u, 0x002Au, 0x002Bu, 0x060Cu, 0x002Du, 0x002Eu, 0x002Fu,
  0x0030u, 0x0031u, 0x0032u, 0x0033u, 0x0034u, 0x0035u, 0x0036u, 0x0037u,
  0x0038u, 0x0039u, 0x003Au, 0x061Bu, 0x003Eu, 0x003Du, 0x003Cu, 0x061Fu,
  0xE620u, 0xE621u, 0xE622u, 0xE623u, 0xE624u, 0xE625u, 0xE626u, 0xE627u,
  0xE628u, 0xE629u, 0xE62Au, 0xE62Bu, 0xE62Cu, 0xE62Du, 0xE62Eu, 0xE62Fu,
  0xE630u, 0xE631u, 0xE632u, 0xE633u, 0xE634u, 0xE635u, 0xE636u, 0xE637u,
  0xE638u, 0xE639u, 0xE63Au, 0xE63Bu, 0xE63Cu, 0xE63Du, 0xE63Eu, 0x0023u,
  0xE640u, 0xE641u, 0xE642u, 0xE643u, 0xE644u, 0xE645u, 0xE646u, 0xE647u,
  0xE648u, 0xE649u, 0xE64Au, 0xE64Bu, 0xE64Cu, 0xE64Du, 0xE64Eu, 0xE64Fu,
  0xE650u, 0xE651u, 0xE652u, 0xE653u, 0xE654u, 0xE655u, 0xE656u, 0xE657u,
  0xE658u, 0xE659u, 0xE65Au, 0xE65Bu, 0xE65Cu, 0xE65Du, 0xE65Eu, 0x25A0u
};

/*
 *  ETS 300 706 Table 45: Arabic G2 Supplementary Set
 *
 *  XXX 0X0000 is what?
 *  Until these tables are finished use private codes U+E7xx.
 */
static const u16 arabic_g2_vsb[96] =
{
/*
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0660, 0x0661, 0x0662, 0x0663, 0x0664, 0x0665, 0x0666, 0x0667,
  0x0668, 0x0669, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
*/
  0xE660u, 0xE661u, 0xE662u, 0xE663u, 0xE664u, 0xE665u, 0xE666u, 0xE667u,
  0xE668u, 0xE669u, 0xE66Au, 0xE66Bu, 0xE66Cu, 0xE66Du, 0xE66Eu, 0xE66Fu,
  0xE670u, 0xE671u, 0xE672u, 0xE673u, 0xE674u, 0xE675u, 0xE676u, 0xE677u,
  0xE678u, 0xE679u, 0xE67Au, 0xE67Bu, 0xE67Cu, 0xE67Du, 0xE67Eu, 0xE67Fu,
  0x00E0u, 0x0041u, 0x0042u, 0x0043u, 0x0044u, 0x0045u, 0x0046u, 0x0047u,
  0x0048u, 0x0049u, 0x004Au, 0x004Bu, 0x004Cu, 0x004Du, 0x004Eu, 0x004Fu,
  0x0050u, 0x0051u, 0x0052u, 0x0053u, 0x0054u, 0x0055u, 0x0056u, 0x0057u,
  0x0058u, 0x0059u, 0x005Au, 0x00EBu, 0x00EAu, 0x00F9u, 0x00EEu, 0xE75Fu,
  0x00E9u, 0x0061u, 0x0062u, 0x0063u, 0x0064u, 0x0065u, 0x0066u, 0x0067u,
  0x0068u, 0x0069u, 0x006Au, 0x006Bu, 0x006Cu, 0x006Du, 0x006Eu, 0x006Fu,
  0x0070u, 0x0071u, 0x0072u, 0x0073u, 0x0074u, 0x0075u, 0x0076u, 0x0077u,
  0x0078u, 0x0079u, 0x007Au, 0x00E2u, 0x00F4u, 0x00FBu, 0x00E7u, 0x0020u
};

/*
 *  ETS 300 706 Table 46: Hebrew G0 Primary Set
 */
static const u16 hebrew_g0_vsb[37] =
{
  0x2190u, 0x00BDu, 0x2192u, 0x2191u, 0x0023u,
  0x05D0u, 0x05D1u, 0x05D2u, 0x05D3u, 0x05D4u, 0x05D5u, 0x05D6u, 0x05D7u,
  0x05D8u, 0x05D9u, 0x05DAu, 0x05DBu, 0x05DCu, 0x05DDu, 0x05DEu, 0x05DFu,
  0x05E0u, 0x05E1u, 0x05E2u, 0x05E3u, 0x05E4u, 0x05E5u, 0x05E6u, 0x05E7u,
  0x05E8u, 0x05E9u, 0x05EAu, 0x20AAu, 0x2016u, 0x00BEu, 0x00F7u, 0x25A0u
};

/*
 * Translate Teletext character code to Unicode.
 * 
 * Exceptions:
 * ETS 300 706 Table 36 Latin National Subset Turkish character
 * 0x23 Turkish currency symbol is not representable in Unicode,
 * translated to private code U+E800. Was unable to identify all
 * Arabic glyphs in Table 44 and 45 Arabic G0 and G2, these are
 * mapped to private code U+E620 ... U+E67F and U+E720 ... U+E77F
 * respectively. Table 47 G1 Block Mosaic is not representable
 * in Unicode, translated to private code U+EE00 ... U+EE7F.
 * (contiguous form has bit 5 set, separate form cleared).
 * Table 48 G3 Smooth Mosaics and Line Drawing Set is not
 * representable in Unicode, translated to private code U+EF20
 * ... U+EF7F.
 *
 * Note that some Teletext character sets contain complementary
 * Latin characters. For example the Greek capital letters Alpha
 * and Beta are reused as Latin capital letter A and B, while a
 * separate code exists for Latin capital letter C. This function
 * is unable to distinguish between uses, so it will always translate
 * Greek A and B to Alpha and Beta, C to Latin C.
 * 
 * Private codes U+F000 ... U+F7FF are reserved for DRCS.
 */
u32 vbi_teletext_unicode_vsb(ttx_character_set_t s, ttx_national_subset_t n, u32 c)
{
    s32 i = 0;

    MT_ASSERT(c >= 0x20 && c <= 0x7F);

    switch(s)
    {
        case CHAR_SET_LATIN_G0:
            /*really hard to understand the tricky number.
                        Change according to E300706, table 35, note 2
                      */
            //if((0xF8000019UL & (1 << (c & 31))) != 0)
            if((0x23u == c) ||  
               (0x24u == c) ||
               (0x40u == c) ||
               (0x5bu == c) ||
               (0x5cu == c) ||
               (0x5du == c) ||
               (0x5eu == c) ||
               (0x5fu == c) ||
               (0x60u == c) ||
               (0x7bu == c) ||
               (0x7cu == c) ||
               (0x7du == c) ||
               (0x7eu == c))
            {
                if(n > 0)
                {
                    MT_ASSERT(n < 14);

                    for(i = 0; i < 13; i ++)
                        if(c == national_subset_vsb[0][i])
                            return national_subset_vsb[n][i];
                }

                if(c == 0x24)
                    return 0x00A4u;
                else if(c == 0x7C)
                    return 0x00A6u;
                else if(c == 0x7F)
                    return 0x25A0u;
            }

            return c;

        case CHAR_SET_LATIN_G2:
            return latin_g2_vsb[c - 0x20];

        case CHAR_SET_CYRILLIC_1_G0:
            if(c < 0x40)
                return c;
            else
                return cyrillic_1_g0_vsb[c - 0x40];

        case CHAR_SET_CYRILLIC_2_G0:
            if(c == 0x26)
                return 0x044Bu;
            else if(c < 0x40)
                return c;
            else
                return cyrillic_2_g0_vsb[c - 0x40];

        case CHAR_SET_CYRILLIC_3_G0:
            if(c == 0x26)
                return 0x00EFu;
            else if(c < 0x40)
                return c;
            else
                return cyrillic_3_g0[c - 0x40];

        case CHAR_SET_CYRILLIC_G2:
            return cyrillic_g2_vsb[c - 0x20];

        case CHAR_SET_GREEK_G0:
            if(c == 0x3C)
                return 0x00ABu;
            else if(c == 0x3E)
                return 0x00BBu;
            else if(c < 0x40)
                return c;
            else
                return greek_g0_vsb[c - 0x40];

        case CHAR_SET_GREEK_G2:
            return greek_g2_vsb[c - 0x20];

        case CHAR_SET_ARABIC_G0:
            return arabic_g0_vsb[c - 0x20];

        case CHAR_SET_ARABIC_G2:
            return arabic_g2_vsb[c - 0x20];

        case CHAR_SET_HEBREW_G0:
            if(c < 0x5B)
                return c;
            else
                return hebrew_g0_vsb[c - 0x5B];

        case CHAR_SET_BLOCK_MOSAIC_G1:
            /* 0x20 ... 0x3F -> 0xEE00 ... 0xEE1F separated */
            /*                  0xEE20 ... 0xEE3F contiguous */
            /* 0x60 ... 0x7F -> 0xEE40 ... 0xEE5F separated */
            /*                  0xEE60 ... 0xEE7F contiguous */
            MT_ASSERT(c < 0x40 || c >= 0x60);
            return 0xEE00u + c;

        case CHAR_SET_SMOOTH_MOSAIC_G3:
            return 0xEF00u + c;

        default:
            OS_PRINTF("TTX!: unknown char set %d\n", s);
            break;
    }

    return 0;
}

/*
 *  Unicode 0x00C0u ... 0x017Fu to
 *  Teletext accent ((Latin G2 0x40 ... 0x4F) - 0x40) << 12 + Latin G0
 */
static const u16 composed_vsb[12 * 16] =
{
    0x1041, 0x2041, 0x3041, 0x4041, 0x8041, 0xA041, 0x0000, 0xB043,
    0x1045, 0x2045, 0x3045, 0x8045, 0x1049, 0x2049, 0x3049, 0x8049,
    0x0000, 0x404E, 0x104F, 0x204F, 0x304F, 0x404F, 0x804F, 0x0000,
    0x0000, 0x1055, 0x2055, 0x3055, 0x8055, 0x2059, 0x0000, 0x0000,
    0x1061, 0x2061, 0x3061, 0x4061, 0x8061, 0xA061, 0x0000, 0xB063,
    0x1065, 0x2065, 0x3065, 0x8065, 0x1069, 0x2069, 0x3069, 0x8069,
    0x0000, 0x406E, 0x106F, 0x206F, 0x306F, 0x406F, 0x806F, 0x0000,
    0x00F9, 0x1075, 0x2075, 0x3075, 0x8075, 0x2079, 0x0000, 0x8079,
    0x5041, 0x5061, 0x6041, 0x6061, 0xE041, 0xE061, 0x2043, 0x2063,
    0x3043, 0x3063, 0x7043, 0x7063, 0xF043, 0xF063, 0xF044, 0xF064,
    0x0000, 0x0000, 0x5045, 0x5065, 0x6045, 0x6065, 0x7045, 0x7065,
    0xE045, 0xE065, 0xF045, 0xF065, 0x3047, 0x3067, 0x6047, 0x6067,
    0x7047, 0x7067, 0x0000, 0x0000, 0x3048, 0x3068, 0x0000, 0x0000,
    0x4049, 0x4069, 0x5049, 0x5069, 0x6049, 0x6069, 0xE049, 0xE069,
    0x7049, 0x0000, 0x0000, 0x0000, 0x304A, 0x306A, 0x0000, 0x0000,
    0x0000, 0x204C, 0x2049, 0x0000, 0x0000, 0xF04C, 0xF06C, 0x0000,
    0x0000, 0x0000, 0x0000, 0x204E, 0x206E, 0x0000, 0x0000, 0xF04E,
    0xF06E, 0x0000, 0x0000, 0x0000, 0x504F, 0x506F, 0x604F, 0x606F,
    0xD04F, 0xD06F, 0x0000, 0x0000, 0x2052, 0x2072, 0x0000, 0x0000,
    0xF052, 0xF072, 0x2053, 0x2073, 0x3053, 0x3073, 0xB053, 0xB073,
    0xF053, 0xF073, 0xB054, 0xB074, 0xF053, 0xF073, 0x0000, 0x0000,
    0x4055, 0x4075, 0x5055, 0x5075, 0x6055, 0x6075, 0xA055, 0xA075,
    0xD055, 0xD075, 0xE055, 0xE075, 0x3057, 0x3077, 0x3059, 0x3079,
    0x8059, 0x205A, 0x207A, 0x705A, 0x707A, 0xF05A, 0xF07A, 0x0000
};

/*
 * Translate Teletext Latin G1 character 0x20 ... 0x7F combined
 * with accent code from Latin G2 0x40 ... 0x4F to Unicode. Not all
 * combinations are representable in Unicode.
 */
u32 vbi_teletext_composed_unicode_vsb(u32 a, u32 c)
{
    u32 i = 0;

    MT_ASSERT(a <= 15);
    MT_ASSERT(c >= 0x20 && c <= 0x7F);

    if(a == 0)
        return vbi_teletext_unicode_vsb(CHAR_SET_LATIN_G0, CHAR_SUBSET_NONE, c);

    c += a << 12;

    for(i = 0; i < sizeof(composed_vsb) / sizeof(composed_vsb[0]); i ++)
        if(composed_vsb[i] == c)
            return 0x00C0u + i;
    return 0;
}
