#include "XGD/Chem/MolUtil.h"
#include "XGD/Chem/Util.h"

#include <openbabel/plugin.h>
#include <openbabel/format.h>
#include <openbabel/obconversion.h>
#include <openbabel/mol.h>
#include <openbabel/op.h>

#include <spdlog/spdlog.h>

#include <string>
#include <unordered_set>

// TODO: check valid text based formats again
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

std::string xgd::chem::convert(
        const std::string &input,
        const std::string &inputFormat,
        const std::string &outputFormat,
        const bool &gen2D,
        const bool &gen3D
) {
    OpenBabel::OBConversion conv;
    {
        const std::string &inS = inputFormat;
        const std::string &outS = outputFormat;
        auto in = OpenBabel::OBConversion::FindFormat(inS);
        if (!in) {
            spdlog::error("fail to find {} format", inS);
            return "";
        }
        auto out = OpenBabel::OBConversion::FindFormat(outS);
        if (!out) {
            spdlog::error("fail to find {} format", outS);
            return "";
        }
        if (!conv.SetInAndOutFormats(in, out)) {
            spdlog::error("fail to set {} and {} format", inS, outS);
            return "";
        }
    }
    OpenBabel::OBMol mol;
    if (!conv.ReadString(&mol, input)) {
        spdlog::error("fail to read {} as {} format", input, conv.GetInFormat()->GetMIMEType());
        return "";
    }
    if (gen3D && !mol.Has3D()) {
        spdlog::info("convert: gen3D");
        auto op = OpenBabel::OBOp::FindType("gen3D");
        if (!op->Do(&mol, "1")) {
            spdlog::error("fail to run OBOp gen3D");
            return "";
        }
    } else if (gen2D && !mol.Has2D(true)) {
        spdlog::info("convert: gen2D");
        auto op = OpenBabel::OBOp::FindType("2D");
        if (!op->Do(&mol)) {
            spdlog::error("fail to run OBOp 2D");
            return "";
        }
    }
    std::string output = conv.WriteString(&mol, true);
    return output;
}
