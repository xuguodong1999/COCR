#include "ckit/mol_util.h"
#include <openbabel/plugin.h>
#include <unordered_set>

// 禁用了一些显示不了 c1ccccc1 的格式
static std::unordered_set<std::string> FORMAT_WRITE_WHITE_LIST = {
        "acesin", "adf", "alc", "ascii", "bgf", "box", "bs", "c3d1", "c3d2", "cac", "caccrt",
        "cache", "cacint", "can", "cdjson", "cht", "cif", "ck", "cof", "com",
//        "confabreport",
        "CONFIG", "CONTCAR", "CONTFF",
//        "copy",
        "crk2d", "crk3d",
//        "csr",
        "cssr", "ct",
//        "cub", "cube",
        "dalmol", "dmol",
//        "dx",
        "ent", "exyz",
//        "fa", "fasta",
        "feat", "fh", "fhiaims", "fix", "fps", "fpt", "fract",
//        "fs", "fsa",
        "gamin", "gau", "gjc", "gjf", "gpr", "gr96", "gro", "gukin",
//        "gukout",
        "gzmat", "hin", "inchi", "inchikey", "inp", "jin", "k", "lmpdat",
//        "lpmd",
        "mae", "mcdl", "mcif", "MDFF", "mdl", "ml2", "mmcif", "mmd", "mmod", "mna", "mol", "mol2", "mold", "molden",
        "molf", "molreport", "mop", "mopcrt", "mopin", "mp", "mpc", "mpd", "mpqcin", "msms",
//        "nul",
        "nw", "orcainp", "outmol", "paint", "pcjson", "pcm", "pdb", "pdbqt", "pointcloud", "POSCAR", "POSFF",
//        "pov",
        "pqr", "pqs", "qcin", "report",
//        "rinchi", "rsmi", "rxn",
        "sd", "sdf", "smi", "smiles",
//        "stl",
        "svg", "sy2",
//        "tdd", "text", "therm",
        "tmol",
//        "txt",
        "txyz", "unixyz",
        "VASP", "vmol", "xed", "xyz",
//        "yob",
        "zin"
};


bool MolUtil::IsValidWritableFormat(const std::string &suffix) {
    return FORMAT_WRITE_WHITE_LIST.end() != FORMAT_WRITE_WHITE_LIST.find(suffix);
}

std::vector<std::string> MolUtil::GetWritableFormats() {
    std::vector<std::string> result;
    OpenBabel::OBPlugin::ListAsVector("formats", nullptr, result);
    return result;
}
