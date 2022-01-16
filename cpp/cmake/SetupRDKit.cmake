find_package(Threads)
find_package(maeparser REQUIRED)
find_package(coordgen REQUIRED)
find_package(rdkit REQUIRED)
set(RDKIT_LIBRARIES RDKit::Abbreviations RDKit::MolCatalog RDKit::Alignment
        RDKit::MolChemicalFeatures RDKit::Catalogs RDKit::MolDraw2D
        RDKit::ChemicalFeatures RDKit::MolEnumerator RDKit::ChemReactions
        RDKit::MolHash RDKit::ChemTransforms RDKit::MolInterchange
        RDKit::CIPLabeler RDKit::MolStandardize RDKit::DataStructs
        RDKit::MolTransforms RDKit::Depictor RDKit::O3AAlign
        RDKit::Deprotect RDKit::Optimizer RDKit::Descriptors
        RDKit::PartialCharges RDKit::DistGeometry RDKit::RDGeneral
        RDKit::DistGeomHelpers RDKit::RDGeometryLib RDKit::EigenSolvers
        RDKit::ReducedGraphs RDKit::FileParsers RDKit::RGroupDecomposition
        RDKit::FilterCatalog RDKit::RingDecomposerLib RDKit::Fingerprints
        RDKit::ScaffoldNetwork RDKit::FMCS RDKit::ShapeHelpers
        RDKit::ForceField RDKit::SimDivPickers RDKit::ForceFieldHelpers
        RDKit::SLNParse RDKit::FragCatalog RDKit::SmilesParse RDKit::GraphMol
        RDKit::Subgraphs RDKit::hc RDKit::SubstructLibrary RDKit::InfoTheory
        RDKit::SubstructMatch RDKit::MMPA RDKit::TautomerQuery RDKit::MolAlign
        RDKit::Trajectory)