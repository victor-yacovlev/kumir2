#ifndef MODELTYPE_H
#define MODELTYPE_H

namespace DocBookViewer {

enum ModelType {
    Unknown,

    Text,
    Book,
    Article,
    Set,
    Chapter,
    Section,
    Preface,
    Reference,
    Abstract,
    Para,
    Example,
    ProgramListing,
    Code,
    OrderedList,
    ItemizedList,
    ListItem,
    Xref,
    Emphasis,
    XInfo,
    KeywordSet,
    Keyword,
    Synopsis,
    KeyCombo,
    KeySym,
    Table,
    InformalTable,
    THead,
    TBody,
    Row,
    Entry,
    MediaObject,
    Caption,
    InlineMediaObject,
    ImageObject,
    ImageData,
    Subscript,
    Superscript,

    FuncSynopsys,
    FuncSynopsysInfo,
    FuncPrototype,
    FuncDef,
    ParamDef,
    Function,
    Parameter,
    Package,
    Type,

    MathML_Math,
    MathML_MRow,
    MathML_MSqrt,
    MathML_MFrac,
    MathML_MI,
    MathML_MN,
    MathML_MO,
    MathML_MText,
    MathML_MSup,

    GuiMenu,
    GuiMenuItem,
    GuiButton,

    // virtual entries
    ListOfExamples,
    ListOfTables,
    ListOfFunctions
};

}

#endif // MODELTYPE_H
