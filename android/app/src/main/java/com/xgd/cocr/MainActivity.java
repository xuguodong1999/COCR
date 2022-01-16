package com.xgd.cocr;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import org.openscience.cdk.DefaultChemObjectBuilder;
import org.openscience.cdk.atomtype.CDKAtomTypeMatcher;
import org.openscience.cdk.exception.CDKException;
import org.openscience.cdk.interfaces.IAtom;
import org.openscience.cdk.interfaces.IAtomContainer;
import org.openscience.cdk.interfaces.IAtomType;
import org.openscience.cdk.io.PDBWriter;
import org.openscience.cdk.modeling.builder3d.ModelBuilder3D;
import org.openscience.cdk.modeling.builder3d.TemplateHandler3D;
import org.openscience.cdk.smiles.SmilesParser;
import org.openscience.cdk.tools.CDKHydrogenAdder;
import org.openscience.cdk.tools.manipulator.AtomContainerManipulator;

import java.io.StringWriter;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";

  void addImplicitHydrogens(IAtomContainer container) throws Exception {
        CDKAtomTypeMatcher matcher = CDKAtomTypeMatcher.getInstance(container.getBuilder());
        int atomCount = container.getAtomCount();
        String[] originalAtomTypeNames = new String[atomCount];
        for (int i = 0; i < atomCount; i++) {
            IAtom atom = container.getAtom(i);
            IAtomType type = matcher.findMatchingAtomType(container, atom);
            originalAtomTypeNames[i] = atom.getAtomTypeName();
            atom.setAtomTypeName(type.getAtomTypeName());
        }
        CDKHydrogenAdder hAdder = CDKHydrogenAdder.getInstance(container.getBuilder());
        hAdder.addImplicitHydrogens(container);
        // reset to the original atom types
        for (int i = 0; i < atomCount; i++) {
            IAtom atom = container.getAtom(i);
            atom.setAtomTypeName(originalAtomTypeNames[i]);
        }
    }

    void addExplicitHydrogens(IAtomContainer container) throws Exception {
        addImplicitHydrogens(container);
        AtomContainerManipulator.convertImplicitToExplicitHydrogens(container);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        try {
            // cdk MM2 力场 会崩溃的情况：
            // 1、一个原子
            // 2、双氧水
            // cdk MMFF94 力场 会崩溃的情况：
            // 1、一个原子
            ModelBuilder3D mb3d = ModelBuilder3D.getInstance(TemplateHandler3D.getInstance(),
                    "mmff94", DefaultChemObjectBuilder.getInstance());
            SmilesParser sp = new SmilesParser(DefaultChemObjectBuilder.getInstance());
            IAtomContainer mol = sp.parseSmiles("CCCCCCCCCC");
            addExplicitHydrogens(mol);
            mol = mb3d.generate3DCoordinates(mol, false);
            StringWriter sWriter = new StringWriter();
            PDBWriter writer = new PDBWriter(sWriter);
            writer.write(mol);
            writer.close();
            Log.d(TAG, "onCreate: pdb=\n" + sWriter.toString());
            for (int i = 0; i < mol.getAtomCount(); i++) {
                Log.d(TAG, "atom: " + mol.getAtom(i).getPoint3d());
            }
        } catch (CDKException e) {
            e.printStackTrace();
        } catch (CloneNotSupportedException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
