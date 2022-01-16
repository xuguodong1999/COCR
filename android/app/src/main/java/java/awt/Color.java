package java.awt;

//import java.awt.color.ColorSpace;
//import java.awt.geom.AffineTransform;
//import java.awt.geom.Rectangle2D;
//import java.awt.image.ColorModel;
//import java.beans.ConstructorProperties;

import java.io.Serializable;

/**
 * put a error-impl of java.awt.Color here to make
 * org.openscience.cdk.modeling.builder3d.MMFF94BasedParameterSetReader
 * work normally
 */
public class Color implements
//        Paint,
        Serializable {
    public static final Color white = new Color(255, 255, 255);
    public static final Color WHITE;
    public static final Color lightGray;
    public static final Color LIGHT_GRAY;
    public static final Color gray;
    public static final Color GRAY;
    public static final Color darkGray;
    public static final Color DARK_GRAY;
    public static final Color black;
    public static final Color BLACK;
    public static final Color red;
    public static final Color RED;
    public static final Color pink;
    public static final Color PINK;
    public static final Color orange;
    public static final Color ORANGE;
    public static final Color yellow;
    public static final Color YELLOW;
    public static final Color green;
    public static final Color GREEN;
    public static final Color magenta;
    public static final Color MAGENTA;
    public static final Color cyan;
    public static final Color CYAN;
    public static final Color blue;
    public static final Color BLUE;
    int value;
    private float[] frgbvalue;
    private float[] fvalue;
    private float falpha;
    //    private ColorSpace cs;
    private static final long serialVersionUID = 118526816881161077L;
    private static final double FACTOR = 0.7D;

//    private static native void initIDs();

    private static void testColorValueRange(int r, int g, int b, int a) {
        boolean rangeError = false;
        String badComponentString = "";
        if (a < 0 || a > 255) {
            rangeError = true;
            badComponentString = badComponentString + " Alpha";
        }

        if (r < 0 || r > 255) {
            rangeError = true;
            badComponentString = badComponentString + " Red";
        }

        if (g < 0 || g > 255) {
            rangeError = true;
            badComponentString = badComponentString + " Green";
        }

        if (b < 0 || b > 255) {
            rangeError = true;
            badComponentString = badComponentString + " Blue";
        }

        if (rangeError) {
            throw new IllegalArgumentException("Color parameter outside of expected range:" + badComponentString);
        }
    }

    private static void testColorValueRange(float r, float g, float b, float a) {
        boolean rangeError = false;
        String badComponentString = "";
        if ((double) a < 0.0D || (double) a > 1.0D) {
            rangeError = true;
            badComponentString = badComponentString + " Alpha";
        }

        if ((double) r < 0.0D || (double) r > 1.0D) {
            rangeError = true;
            badComponentString = badComponentString + " Red";
        }

        if ((double) g < 0.0D || (double) g > 1.0D) {
            rangeError = true;
            badComponentString = badComponentString + " Green";
        }

        if ((double) b < 0.0D || (double) b > 1.0D) {
            rangeError = true;
            badComponentString = badComponentString + " Blue";
        }

        if (rangeError) {
            throw new IllegalArgumentException("Color parameter outside of expected range:" + badComponentString);
        }
    }

    public Color(int r, int g, int b) {
        this(r, g, b, 255);
    }

    //    @ConstructorProperties({"red", "green", "blue", "alpha"})
    public Color(int r, int g, int b, int a) {
        this.frgbvalue = null;
        this.fvalue = null;
        this.falpha = 0.0F;
//        this.cs = null;
        this.value = (a & 255) << 24 | (r & 255) << 16 | (g & 255) << 8 | (b & 255) << 0;
        testColorValueRange(r, g, b, a);
    }

    public Color(int rgb) {
        this.frgbvalue = null;
        this.fvalue = null;
        this.falpha = 0.0F;
//        this.cs = null;
        this.value = -16777216 | rgb;
    }

    public Color(int rgba, boolean hasalpha) {
        this.frgbvalue = null;
        this.fvalue = null;
        this.falpha = 0.0F;
//        this.cs = null;
        if (hasalpha) {
            this.value = rgba;
        } else {
            this.value = -16777216 | rgba;
        }

    }

    public Color(float r, float g, float b) {
        this((int) ((double) (r * 255.0F) + 0.5D), (int) ((double) (g * 255.0F) + 0.5D), (int) ((double) (b * 255.0F) + 0.5D));
        testColorValueRange(r, g, b, 1.0F);
        this.frgbvalue = new float[3];
        this.frgbvalue[0] = r;
        this.frgbvalue[1] = g;
        this.frgbvalue[2] = b;
        this.falpha = 1.0F;
        this.fvalue = this.frgbvalue;
    }

    public Color(float r, float g, float b, float a) {
        this((int) ((double) (r * 255.0F) + 0.5D), (int) ((double) (g * 255.0F) + 0.5D), (int) ((double) (b * 255.0F) + 0.5D), (int) ((double) (a * 255.0F) + 0.5D));
        this.frgbvalue = new float[3];
        this.frgbvalue[0] = r;
        this.frgbvalue[1] = g;
        this.frgbvalue[2] = b;
        this.falpha = a;
        this.fvalue = this.frgbvalue;
    }

//    public Color(ColorSpace cspace, float[] components, float alpha) {
//        this.frgbvalue = null;
//        this.fvalue = null;
//        this.falpha = 0.0F;
//        this.cs = null;
//        boolean rangeError = false;
//        String badComponentString = "";
//        int n = cspace.getNumComponents();
//        this.fvalue = new float[n];
//
//        for(int i = 0; i < n; ++i) {
//            if ((double)components[i] >= 0.0D && (double)components[i] <= 1.0D) {
//                this.fvalue[i] = components[i];
//            } else {
//                rangeError = true;
//                badComponentString = badComponentString + "Component " + i + " ";
//            }
//        }
//
//        if ((double)alpha >= 0.0D && (double)alpha <= 1.0D) {
//            this.falpha = alpha;
//        } else {
//            rangeError = true;
//            badComponentString = badComponentString + "Alpha";
//        }
//
//        if (rangeError) {
//            throw new IllegalArgumentException("Color parameter outside of expected range: " + badComponentString);
//        } else {
//            this.frgbvalue = cspace.toRGB(this.fvalue);
//            this.cs = cspace;
//            this.value = ((int)(this.falpha * 255.0F) & 255) << 24 | ((int)(this.frgbvalue[0] * 255.0F) & 255) << 16 | ((int)(this.frgbvalue[1] * 255.0F) & 255) << 8 | ((int)(this.frgbvalue[2] * 255.0F) & 255) << 0;
//        }
//    }

    public int getRed() {
        return this.getRGB() >> 16 & 255;
    }

    public int getGreen() {
        return this.getRGB() >> 8 & 255;
    }

    public int getBlue() {
        return this.getRGB() >> 0 & 255;
    }

    public int getAlpha() {
        return this.getRGB() >> 24 & 255;
    }

    public int getRGB() {
        return this.value;
    }

    public Color brighter() {
        int r = this.getRed();
        int g = this.getGreen();
        int b = this.getBlue();
        int alpha = this.getAlpha();
        int i = 3;
        if (r == 0 && g == 0 && b == 0) {
            return new Color(i, i, i, alpha);
        } else {
            if (r > 0 && r < i) {
                r = i;
            }

            if (g > 0 && g < i) {
                g = i;
            }

            if (b > 0 && b < i) {
                b = i;
            }

            return new Color(Math.min((int) ((double) r / 0.7D), 255), Math.min((int) ((double) g / 0.7D), 255), Math.min((int) ((double) b / 0.7D), 255), alpha);
        }
    }

    public Color darker() {
        return new Color(Math.max((int) ((double) this.getRed() * 0.7D), 0), Math.max((int) ((double) this.getGreen() * 0.7D), 0), Math.max((int) ((double) this.getBlue() * 0.7D), 0), this.getAlpha());
    }

    public int hashCode() {
        return this.value;
    }

    public boolean equals(Object obj) {
        return obj instanceof Color && ((Color) obj).getRGB() == this.getRGB();
    }

    public String toString() {
        String var10000 = this.getClass().getName();
        return var10000 + "[r=" + this.getRed() + ",g=" + this.getGreen() + ",b=" + this.getBlue() + "]";
    }

    public static Color decode(String nm) throws NumberFormatException {
        Integer intval = Integer.decode(nm);
        int i = intval;
        return new Color(i >> 16 & 255, i >> 8 & 255, i & 255);
    }

    public static Color getColor(String nm) {
        return getColor(nm, (Color) null);
    }

    public static Color getColor(String nm, Color v) {
        Integer intval = Integer.getInteger(nm);
        if (intval == null) {
            return v;
        } else {
            int i = intval;
            return new Color(i >> 16 & 255, i >> 8 & 255, i & 255);
        }
    }

    public static Color getColor(String nm, int v) {
        Integer intval = Integer.getInteger(nm);
        int i = intval != null ? intval : v;
        return new Color(i >> 16 & 255, i >> 8 & 255, i >> 0 & 255);
    }

    public static int HSBtoRGB(float hue, float saturation, float brightness) {
        int r = 0;
        int g = 0;
        int b = 0;
        if (saturation == 0.0F) {
            r = g = b = (int) (brightness * 255.0F + 0.5F);
        } else {
            float h = (hue - (float) Math.floor((double) hue)) * 6.0F;
            float f = h - (float) Math.floor((double) h);
            float p = brightness * (1.0F - saturation);
            float q = brightness * (1.0F - saturation * f);
            float t = brightness * (1.0F - saturation * (1.0F - f));
            switch ((int) h) {
                case 0:
                    r = (int) (brightness * 255.0F + 0.5F);
                    g = (int) (t * 255.0F + 0.5F);
                    b = (int) (p * 255.0F + 0.5F);
                    break;
                case 1:
                    r = (int) (q * 255.0F + 0.5F);
                    g = (int) (brightness * 255.0F + 0.5F);
                    b = (int) (p * 255.0F + 0.5F);
                    break;
                case 2:
                    r = (int) (p * 255.0F + 0.5F);
                    g = (int) (brightness * 255.0F + 0.5F);
                    b = (int) (t * 255.0F + 0.5F);
                    break;
                case 3:
                    r = (int) (p * 255.0F + 0.5F);
                    g = (int) (q * 255.0F + 0.5F);
                    b = (int) (brightness * 255.0F + 0.5F);
                    break;
                case 4:
                    r = (int) (t * 255.0F + 0.5F);
                    g = (int) (p * 255.0F + 0.5F);
                    b = (int) (brightness * 255.0F + 0.5F);
                    break;
                case 5:
                    r = (int) (brightness * 255.0F + 0.5F);
                    g = (int) (p * 255.0F + 0.5F);
                    b = (int) (q * 255.0F + 0.5F);
            }
        }

        return -16777216 | r << 16 | g << 8 | b << 0;
    }

    public static float[] RGBtoHSB(int r, int g, int b, float[] hsbvals) {
        if (hsbvals == null) {
            hsbvals = new float[3];
        }

        int cmax = r > g ? r : g;
        if (b > cmax) {
            cmax = b;
        }

        int cmin = r < g ? r : g;
        if (b < cmin) {
            cmin = b;
        }

        float brightness = (float) cmax / 255.0F;
        float saturation;
        if (cmax != 0) {
            saturation = (float) (cmax - cmin) / (float) cmax;
        } else {
            saturation = 0.0F;
        }

        float hue;
        if (saturation == 0.0F) {
            hue = 0.0F;
        } else {
            float redc = (float) (cmax - r) / (float) (cmax - cmin);
            float greenc = (float) (cmax - g) / (float) (cmax - cmin);
            float bluec = (float) (cmax - b) / (float) (cmax - cmin);
            if (r == cmax) {
                hue = bluec - greenc;
            } else if (g == cmax) {
                hue = 2.0F + redc - bluec;
            } else {
                hue = 4.0F + greenc - redc;
            }

            hue /= 6.0F;
            if (hue < 0.0F) {
                ++hue;
            }
        }

        hsbvals[0] = hue;
        hsbvals[1] = saturation;
        hsbvals[2] = brightness;
        return hsbvals;
    }

    public static Color getHSBColor(float h, float s, float b) {
        return new Color(HSBtoRGB(h, s, b));
    }

    public float[] getRGBComponents(float[] compArray) {
        float[] f;
        if (compArray == null) {
            f = new float[4];
        } else {
            f = compArray;
        }

        if (this.frgbvalue == null) {
            f[0] = (float) this.getRed() / 255.0F;
            f[1] = (float) this.getGreen() / 255.0F;
            f[2] = (float) this.getBlue() / 255.0F;
            f[3] = (float) this.getAlpha() / 255.0F;
        } else {
            f[0] = this.frgbvalue[0];
            f[1] = this.frgbvalue[1];
            f[2] = this.frgbvalue[2];
            f[3] = this.falpha;
        }

        return f;
    }

    public float[] getRGBColorComponents(float[] compArray) {
        float[] f;
        if (compArray == null) {
            f = new float[3];
        } else {
            f = compArray;
        }

        if (this.frgbvalue == null) {
            f[0] = (float) this.getRed() / 255.0F;
            f[1] = (float) this.getGreen() / 255.0F;
            f[2] = (float) this.getBlue() / 255.0F;
        } else {
            f[0] = this.frgbvalue[0];
            f[1] = this.frgbvalue[1];
            f[2] = this.frgbvalue[2];
        }

        return f;
    }

    public float[] getComponents(float[] compArray) {
        if (this.fvalue == null) {
            return this.getRGBComponents(compArray);
        } else {
            int n = this.fvalue.length;
            float[] f;
            if (compArray == null) {
                f = new float[n + 1];
            } else {
                f = compArray;
            }

            for (int i = 0; i < n; ++i) {
                f[i] = this.fvalue[i];
            }

            f[n] = this.falpha;
            return f;
        }
    }

    public float[] getColorComponents(float[] compArray) {
        if (this.fvalue == null) {
            return this.getRGBColorComponents(compArray);
        } else {
            int n = this.fvalue.length;
            float[] f;
            if (compArray == null) {
                f = new float[n];
            } else {
                f = compArray;
            }

            for (int i = 0; i < n; ++i) {
                f[i] = this.fvalue[i];
            }

            return f;
        }
    }

//    public float[] getComponents(ColorSpace cspace, float[] compArray) {
//        if (this.cs == null) {
//            this.cs = ColorSpace.getInstance(1000);
//        }
//
//        float[] f;
//        if (this.fvalue == null) {
//            f = new float[]{(float)this.getRed() / 255.0F, (float)this.getGreen() / 255.0F, (float)this.getBlue() / 255.0F};
//        } else {
//            f = this.fvalue;
//        }
//
//        float[] tmp = this.cs.toCIEXYZ(f);
//        float[] tmpout = cspace.fromCIEXYZ(tmp);
//        if (compArray == null) {
//            compArray = new float[tmpout.length + 1];
//        }
//
//        for(int i = 0; i < tmpout.length; ++i) {
//            compArray[i] = tmpout[i];
//        }
//
//        if (this.fvalue == null) {
//            compArray[tmpout.length] = (float)this.getAlpha() / 255.0F;
//        } else {
//            compArray[tmpout.length] = this.falpha;
//        }
//
//        return compArray;
//    }
//
//    public float[] getColorComponents(ColorSpace cspace, float[] compArray) {
//        if (this.cs == null) {
//            this.cs = ColorSpace.getInstance(1000);
//        }
//
//        float[] f;
//        if (this.fvalue == null) {
//            f = new float[]{(float)this.getRed() / 255.0F, (float)this.getGreen() / 255.0F, (float)this.getBlue() / 255.0F};
//        } else {
//            f = this.fvalue;
//        }
//
//        float[] tmp = this.cs.toCIEXYZ(f);
//        float[] tmpout = cspace.fromCIEXYZ(tmp);
//        if (compArray == null) {
//            return tmpout;
//        } else {
//            for(int i = 0; i < tmpout.length; ++i) {
//                compArray[i] = tmpout[i];
//            }
//
//            return compArray;
//        }
//    }
//
//    public ColorSpace getColorSpace() {
//        if (this.cs == null) {
//            this.cs = ColorSpace.getInstance(1000);
//        }
//
//        return this.cs;
//    }
//
//    public synchronized PaintContext createContext(ColorModel cm, Rectangle r, Rectangle2D r2d, AffineTransform xform, RenderingHints hints) {
//        return new ColorPaintContext(this.getRGB(), cm);
//    }

    public int getTransparency() {
        int alpha = this.getAlpha();
        if (alpha == 255) {
            return 1;
        } else {
            return alpha == 0 ? 2 : 3;
        }
    }

    static {
        WHITE = white;
        lightGray = new Color(192, 192, 192);
        LIGHT_GRAY = lightGray;
        gray = new Color(128, 128, 128);
        GRAY = gray;
        darkGray = new Color(64, 64, 64);
        DARK_GRAY = darkGray;
        black = new Color(0, 0, 0);
        BLACK = black;
        red = new Color(255, 0, 0);
        RED = red;
        pink = new Color(255, 175, 175);
        PINK = pink;
        orange = new Color(255, 200, 0);
        ORANGE = orange;
        yellow = new Color(255, 255, 0);
        YELLOW = yellow;
        green = new Color(0, 255, 0);
        GREEN = green;
        magenta = new Color(255, 0, 255);
        MAGENTA = magenta;
        cyan = new Color(0, 255, 255);
        CYAN = cyan;
        blue = new Color(0, 0, 255);
        BLUE = blue;
//        Toolkit.loadLibraries();
//        if (!GraphicsEnvironment.isHeadless()) {
//            initIDs();
//        }

    }
}
