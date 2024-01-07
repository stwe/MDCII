// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

package mdcii;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;

import static mdcii.Log.LOGGER;

/**
 * Class to create Tile Atlas Images.
 */
public class AtlasCreator {

    //-------------------------------------------------
    // Member
    //-------------------------------------------------

    /**
     * A {@link PaletteFile} object.
     */
    PaletteFile paletteFile;

    //-------------------------------------------------
    // Ctors.
    //-------------------------------------------------

    /**
     * Constructs a new {@link AtlasCreator} object.
     *
     * @throws IOException If an I/O error is thrown.
     * @throws IllegalAccessException If an illegal access exception is thrown.
     */
    AtlasCreator() throws IOException, IllegalAccessException {
        ConfigLoader.load(CreatorConfig.class, "/creator.properties");

        LOGGER.debug("Configuration loaded successfully.");

        LOGGER.info("-----------------------------------------------");
        LOGGER.info("|       MDCII Tile Atlas Images Creator       |");
        LOGGER.info("-----------------------------------------------");
        LOGGER.info("Root path: {}", CreatorConfig.ROOT_PATH);
        LOGGER.info("Create Pngs: {}", CreatorConfig.CREATE_PNGS);
        LOGGER.info("Png out path: {}", CreatorConfig.PNG_OUT_PATH);
        LOGGER.info("Atlas out path: {}", CreatorConfig.ATLAS_OUT_PATH);
        LOGGER.info("-----------------------------------------------");
    }

    //-------------------------------------------------
    // Run
    //-------------------------------------------------

    public void run() throws IOException {
        // palette
        LOGGER.info("Read the color palette from file ...");
        paletteFile = new PaletteFile(Paths.get(CreatorConfig.ROOT_PATH + "/ToolGfx/STADTFLD.COL"));
        LOGGER.info("The palette was read successfully.");

        // animals
        LOGGER.info("Start Tiere Tile Atlas Images creation ...");
        createAnimalsSgfxAtlas();
        createAnimalsMgfxAtlas();
        createAnimalsGfxAtlas();
        LOGGER.info("The Tiere Tile Atlas Images have been created successfully.");

        // buildings
        LOGGER.info("Start Stadtfld Tile Atlas Images creation ...");
        createSgfxAtlas();
        createMgfxAtlas();
        createGfxAtlas();
        LOGGER.info("The Stadtfld Tile Atlas Images have been created successfully.");
    }

    //-------------------------------------------------
    // Animals
    //-------------------------------------------------

    private void createAnimalsSgfxAtlas() throws IOException {
        LOGGER.info("Tile Atlas Animals SGFX Images creation ...");

        var bshFile = new BshFile(
                Paths.get(CreatorConfig.ROOT_PATH + "/SGFX/Tiere.bsh"),
                paletteFile.getPalette(),
                BshFile.Zoom.SGFX,
                CreatorConfig.CREATE_PNGS
        );
        var bshTextures = bshFile.getBshTextures();
        var atlasImages = new ArrayList<BufferedImage>();
        var heights = new ArrayList<Integer>();

        var c = 0;
        for (var i = 0; i < TileAtlas.NR_OF_ANIMALS_ATLAS_IMAGES; i++) {
            // new atlas
            var atlas = new BufferedImage(
                    bshFile.getMaxX() * TileAtlas.NR_OF_ANIMALS_ROWS,
                    bshFile.getMaxY() * TileAtlas.NR_OF_ANIMALS_ROWS,
                    BufferedImage.TYPE_INT_ARGB
            );

            // draw bsh images
            for (var y = 0; y < TileAtlas.NR_OF_ANIMALS_ROWS; y++) {
                for (var x = 0; x < TileAtlas.NR_OF_ANIMALS_ROWS; x++) {
                    var g = atlas.getGraphics();

                    // only if index exists
                    if (c >= 0 && c < bshTextures.size()) {
                        // draw in atlas
                        g.drawImage(
                                bshTextures.get(c).getBufferedImage(),
                                x * bshFile.getMaxX(),
                                y * bshFile.getMaxY(),
                                null
                        );

                        heights.add(bshTextures.get(c).getBufferedImage().getHeight());
                    }

                    c++;
                }
            }

            // store atlas
            atlasImages.add(atlas);
        }

        var zoomStr = BshFile.Zoom.SGFX.toString().toLowerCase() + "/animals/";
        writeAtlasImages(
                CreatorConfig.ATLAS_OUT_PATH + zoomStr,
                atlasImages,
                bshFile.getMaxX(),
                bshFile.getMaxY(),
                heights
        );

        bshFile.cleanUp();
    }

    private void createAnimalsMgfxAtlas() throws IOException {
        LOGGER.info("Tile Atlas Animals MGFX Images creation ...");

        var bshFile = new BshFile(
                Paths.get(CreatorConfig.ROOT_PATH + "/MGFX/TIERE.BSH"),
                paletteFile.getPalette(),
                BshFile.Zoom.MGFX,
                CreatorConfig.CREATE_PNGS
        );
        var bshTextures = bshFile.getBshTextures();
        var atlasImages = new ArrayList<BufferedImage>();
        var heights = new ArrayList<Integer>();

        var c = 0;
        for (var i = 0; i < TileAtlas.NR_OF_ANIMALS_ATLAS_IMAGES; i++) {
            // new atlas
            var atlas = new BufferedImage(
                    bshFile.getMaxX() * TileAtlas.NR_OF_ANIMALS_ROWS,
                    bshFile.getMaxY() * TileAtlas.NR_OF_ANIMALS_ROWS,
                    BufferedImage.TYPE_INT_ARGB
            );

            // draw bsh images
            for (var y = 0; y < TileAtlas.NR_OF_ANIMALS_ROWS; y++) {
                for (var x = 0; x < TileAtlas.NR_OF_ANIMALS_ROWS; x++) {
                    var g = atlas.getGraphics();

                    // only if index exists
                    if (c >= 0 && c < bshTextures.size()) {
                        // draw in atlas
                        g.drawImage(
                                bshTextures.get(c).getBufferedImage(),
                                x * bshFile.getMaxX(),
                                y * bshFile.getMaxY(),
                                null
                        );

                        heights.add(bshTextures.get(c).getBufferedImage().getHeight());
                    }

                    c++;
                }
            }

            // store atlas
            atlasImages.add(atlas);
        }

        var zoomStr = BshFile.Zoom.MGFX.toString().toLowerCase() + "/animals/";
        writeAtlasImages(
                CreatorConfig.ATLAS_OUT_PATH + zoomStr,
                atlasImages,
                bshFile.getMaxX(),
                bshFile.getMaxY(),
                heights
        );

        bshFile.cleanUp();
    }

    private void createAnimalsGfxAtlas() throws IOException {
        LOGGER.info("Tile Atlas Animals GFX Images creation ...");

        var bshFile = new BshFile(
                Paths.get(CreatorConfig.ROOT_PATH + "/GFX/TIERE.BSH"),
                paletteFile.getPalette(),
                BshFile.Zoom.GFX,
                CreatorConfig.CREATE_PNGS
        );
        var bshTextures = bshFile.getBshTextures();
        var atlasImages = new ArrayList<BufferedImage>();
        var heights = new ArrayList<Integer>();

        var c = 0;
        for (var i = 0; i < TileAtlas.NR_OF_ANIMALS_ATLAS_IMAGES; i++) {
            // new atlas
            var atlas = new BufferedImage(
                    bshFile.getMaxX() * TileAtlas.NR_OF_ANIMALS_ROWS,
                    bshFile.getMaxY() * TileAtlas.NR_OF_ANIMALS_ROWS,
                    BufferedImage.TYPE_INT_ARGB
            );

            // draw bsh images
            for (var y = 0; y < TileAtlas.NR_OF_ANIMALS_ROWS; y++) {
                for (var x = 0; x < TileAtlas.NR_OF_ANIMALS_ROWS; x++) {
                    var g = atlas.getGraphics();

                    // only if index exists
                    if (c >= 0 && c < bshTextures.size()) {
                        // draw in atlas
                        g.drawImage(
                                bshTextures.get(c).getBufferedImage(),
                                x * bshFile.getMaxX(),
                                y * bshFile.getMaxY(),
                                null
                        );

                        heights.add(bshTextures.get(c).getBufferedImage().getHeight());
                    }

                    c++;
                }
            }

            // store atlas
            atlasImages.add(atlas);
        }

        var zoomStr = BshFile.Zoom.GFX.toString().toLowerCase() + "/animals/";
        writeAtlasImages(
                CreatorConfig.ATLAS_OUT_PATH + zoomStr,
                atlasImages,
                bshFile.getMaxX(),
                bshFile.getMaxY(),
                heights
        );

        bshFile.cleanUp();
    }

    //-------------------------------------------------
    // Stadtfld
    //-------------------------------------------------

    /**
     * Creates and stores the SGFX atlas images.
     *
     * @throws IOException If an I/O error is thrown.
     */
    private void createSgfxAtlas() throws IOException {
        LOGGER.info("Tile Atlas SGFX Images creation ...");

        var bshFile = new BshFile(
                Paths.get(CreatorConfig.ROOT_PATH + "/SGFX/Stadtfld.bsh"),
                paletteFile.getPalette(),
                BshFile.Zoom.SGFX,
                CreatorConfig.CREATE_PNGS
        );
        var bshTextures = bshFile.getBshTextures();
        var atlasImages = new ArrayList<BufferedImage>();
        var heights = new ArrayList<Integer>();

        var c = 0;
        for (var i = 0; i < TileAtlas.NR_OF_SGFX_ATLAS_IMAGES; i++) {
            // new atlas
            var atlas = new BufferedImage(
                    bshFile.getMaxX() * TileAtlas.NR_OF_SGFX_ROWS,
                    bshFile.getMaxY() * TileAtlas.NR_OF_SGFX_ROWS,
                    BufferedImage.TYPE_INT_ARGB
            );

            // draw bsh images
            for (var y = 0; y < TileAtlas.NR_OF_SGFX_ROWS; y++) {
                for (var x = 0; x < TileAtlas.NR_OF_SGFX_ROWS; x++) {
                    var g = atlas.getGraphics();

                    // only if index exists
                    if (c >= 0 && c < bshTextures.size()) {
                        // draw in atlas
                        g.drawImage(
                                bshTextures.get(c).getBufferedImage(),
                                x * bshFile.getMaxX(),
                                y * bshFile.getMaxY(),
                                null
                        );

                        heights.add(bshTextures.get(c).getBufferedImage().getHeight());
                    }

                    c++;
                }
            }

            // store atlas
            atlasImages.add(atlas);
        }

        var zoomStr = BshFile.Zoom.SGFX.toString().toLowerCase() + "/stadtfld/";
        writeAtlasImages(
                CreatorConfig.ATLAS_OUT_PATH + zoomStr,
                atlasImages,
                bshFile.getMaxX(),
                bshFile.getMaxY(),
                heights
        );

        bshFile.cleanUp();
    }

    /**
     * Creates and stores the MGFX atlas images.
     *
     * @throws IOException If an I/O error is thrown.
     */
    private void createMgfxAtlas() throws IOException {
        LOGGER.info("Tile Atlas MGFX Images creation ...");

        var bshFile = new BshFile(
                Paths.get(CreatorConfig.ROOT_PATH + "/MGFX/Stadtfld.bsh"),
                paletteFile.getPalette(),
                BshFile.Zoom.MGFX,
                CreatorConfig.CREATE_PNGS
        );
        var bshTextures = bshFile.getBshTextures();
        var atlasImages = new ArrayList<BufferedImage>();
        var heights = new ArrayList<Integer>();

        var c = 0;
        for (var i = 0; i < TileAtlas.NR_OF_MGFX_ATLAS_IMAGES; i++) {
            // new atlas
            var atlas = new BufferedImage(
                    bshFile.getMaxX() * TileAtlas.NR_OF_MGFX_ROWS,
                    bshFile.getMaxY() * TileAtlas.NR_OF_MGFX_ROWS,
                    BufferedImage.TYPE_INT_ARGB
            );

            // draw bsh images
            for (var y = 0; y < TileAtlas.NR_OF_MGFX_ROWS; y++) {
                for (var x = 0; x < TileAtlas.NR_OF_MGFX_ROWS; x++) {
                    var g = atlas.getGraphics();

                    // only if index exists
                    if (c >= 0 && c < bshTextures.size()) {
                        // draw in atlas
                        g.drawImage(
                                bshTextures.get(c).getBufferedImage(),
                                x * bshFile.getMaxX(),
                                y * bshFile.getMaxY(),
                                null
                        );

                        heights.add(bshTextures.get(c).getBufferedImage().getHeight());
                    }

                    c++;
                }
            }

            // store atlas
            atlasImages.add(atlas);
        }

        var zoomStr = BshFile.Zoom.MGFX.toString().toLowerCase() + "/stadtfld/";
        writeAtlasImages(
                CreatorConfig.ATLAS_OUT_PATH + zoomStr,
                atlasImages,
                bshFile.getMaxX(),
                bshFile.getMaxY(),
                heights
        );

        bshFile.cleanUp();
    }

    /**
     * Creates and stores the GFX atlas images.
     *
     * @throws IOException If an I/O error is thrown.
     */
    private void createGfxAtlas() throws IOException {
        LOGGER.info("Tile Atlas GFX Images creation ...");

        var bshFile = new BshFile(
                Paths.get(CreatorConfig.ROOT_PATH + "/GFX/STADTFLD.BSH"),
                paletteFile.getPalette(),
                BshFile.Zoom.GFX,
                CreatorConfig.CREATE_PNGS
        );
        var bshTextures = bshFile.getBshTextures();
        var atlasImages = new ArrayList<BufferedImage>();
        var heights = new ArrayList<Integer>();

        var c = 0;
        for (var i = 0; i < TileAtlas.NR_OF_GFX_ATLAS_IMAGES; i++) {
            // new atlas
            var atlas = new BufferedImage(
                    bshFile.getMaxX() * TileAtlas.NR_OF_GFX_ROWS,
                    bshFile.getMaxY() * TileAtlas.NR_OF_GFX_ROWS,
                    BufferedImage.TYPE_INT_ARGB
            );

            // draw bsh images
            for (var y = 0; y < TileAtlas.NR_OF_GFX_ROWS; y++) {
                for (var x = 0; x < TileAtlas.NR_OF_GFX_ROWS; x++) {
                    var g = atlas.getGraphics();

                    // only if index exists
                    if (c >= 0 && c < bshTextures.size()) {
                        // draw in atlas
                        g.drawImage(
                                bshTextures.get(c).getBufferedImage(),
                                x * bshFile.getMaxX(),
                                y * bshFile.getMaxY(),
                                null
                        );

                        heights.add(bshTextures.get(c).getBufferedImage().getHeight());
                    }

                    c++;
                }
            }

            // store atlas
            atlasImages.add(atlas);
        }

        var zoomStr = BshFile.Zoom.GFX.toString().toLowerCase() + "/stadtfld/";
        writeAtlasImages(
                CreatorConfig.ATLAS_OUT_PATH + zoomStr,
                atlasImages,
                bshFile.getMaxX(),
                bshFile.getMaxY(),
                heights
        );

        bshFile.cleanUp();
    }

    //-------------------------------------------------
    // Write
    //-------------------------------------------------

    /**
     * Writes {@link BufferedImage} objects to PNG files.
     *
     * @param path The output path.
     * @param bufferedImages The {@link BufferedImage} objects to be written.
     * @param maxX The max width.
     * @param maxY The max height.
     * @param heights The image heights.
     * @throws IOException If an I/O error is thrown.
     */
    private void writeAtlasImages(
            String path,
            ArrayList<BufferedImage> bufferedImages,
            int maxX, int maxY,
            ArrayList<Integer> heights
    ) throws IOException {
        var c = 0;

        Files.createDirectories(Paths.get(path));

        for (var atlas : bufferedImages) {
            String filename = path + c + ".png";

            var file = new File(filename);
            if (!file.exists()) {
                var result = file.createNewFile();
                if (!result) {
                    throw new RuntimeException("Unexpected error.");
                }
            }

            ImageIO.write(atlas, "PNG", file);

            atlas.getGraphics().dispose();

            c++;
        }

        var infoFile = new File(path + "info.txt");
        var infoFileWriter = new FileWriter(infoFile);
        var output = new BufferedWriter(infoFileWriter);
        output.write("maxX: " + maxX + ", maxY: " + maxY);
        output.close();

        if (!heights.isEmpty()) {
            try (var printWriter = new PrintWriter(new FileWriter(path + "heights.txt"))) {
                for (var o : heights) {
                    printWriter.println(o);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
