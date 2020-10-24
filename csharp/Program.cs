using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using CambridgeSoft.ChemScript19;
namespace ChemScriptCSharp
{
    class Program
    {
        static void Main(string[] args)
        {
            var sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            CambridgeSoft.ChemScript19.Environment.SetVerbosity(false);
            CambridgeSoft.ChemScript19.Environment.SetThrowExceptions(true);
            Console.WriteLine(CambridgeSoft.ChemScript19.Environment.Version());
            foreach (var i in StructureData.MimeTypes())
            {
                Console.WriteLine(i);
            }
            StructureData m = StructureData.LoadData("CCC(CC(C(C)(C)C)C(C(CC)C)C(C)C)CC", "smiles");
            foreach (Atom a in m.Atoms)
            {
                
            }
            foreach (Bond b in m.Bonds)
            {
                
            }
            return;

            StreamReader streamReader = new StreamReader(
                "D:\\22及以下碳的饱和烷烃.txt", Encoding.Default);
            BinaryWriter writer = new BinaryWriter(
                new FileStream("D:\\output.dat", FileMode.Create));
            BinaryReader binaryReader = new BinaryReader(new FileStream("1",FileMode.Open));
            var count = binaryReader.ReadUInt64();
            //StreamWriter writer = new StreamWriter("D:\\output.dat");
            String line;
            int ccc = 0;
            while ((line = streamReader.ReadLine()) != null)
            {
                ccc += 1;
                if (ccc % 1000 == 0)
                {
                    TimeSpan ts2 = sw.Elapsed;
                    Console.WriteLine(ccc);
                    Console.WriteLine(ts2.TotalMilliseconds / ccc + " ms/个");
                }
                StructureData m = StructureData.LoadData(line, "smiles");
                if (m == null || m.IsEmpty)
                {
                    // Console.WriteLine(line);
                    continue;
                }
                writer.Write(m.WriteData("smiles"));
                //Console.WriteLine(m.WriteData("smiles"));
                m.ConvertTo3DStructure(true);
                //m.ConvertToFixedBondRep();
                //m.ConvertToDelocBondRep(); 
                //var op = new NormOptions();
                //op.RemoveLabel = false;
                //m.NormalizeStructure(op);
                //var l = m.AmbiguousStereoAtoms();
                //if (l != null) {
                //    Console.WriteLine(l.ToString());
                //}
                //else
                //{
                //    Console.WriteLine("m.AmbiguousStereoAtoms()=null");
                //}
                //Console.WriteLine(m.DetectBondStereoCIP().ToString());
                //Console.WriteLine(m.DetectAtomStereoCIP().ToString());
                writer.Write(m.Atoms.Count);
                foreach (Atom a in m.Atoms)
                {
                    Point p = a.GetCartesian();
                    writer.Write(a.ID);
                    writer.Write(a.Element);
                    writer.Write(p.x);
                    writer.Write(p.y);
                    writer.Write(a.FormalCharge);
                    //Console.WriteLine(
                    //    a.ID + "\t" +
                    //    a.Element + "\t" +
                    //    p.x + "\t" +
                    //    p.y + "\t" +
                    //    p.z + "\t" +
                    //    a.FormalCharge);
                }
                writer.Write(m.Bonds.Count);
                foreach (Bond b in m.Bonds)
                {
                    writer.Write(b.Atom1.ID);
                    writer.Write(b.Atom2.ID);
                    writer.Write(b.OrderName);
                    writer.Write(b.GStereo.Name);
                    //Console.WriteLine(
                    //    b.Atom1.Name + "\t" +
                    //    b.Atom2.Name + "\t" +
                    //    b.OrderName + "\t" +
                    //    b.GStereoName + "\t");
                }
                m.Dispose();
            }
            streamReader.Close();
            writer.Close();
            sw.Stop();
        }
    }
}
