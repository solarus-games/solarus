/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor;

import java.io.*;
import java.nio.file.*;
import java.nio.file.attribute.*;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Utility class to make easy some common operations related to the files:
 * their content, their name, their path or their extension.
 */
public class FileTools {

    /**
     * This class should not be instanciated.
     */
    private FileTools() {

    }

    /**
     * Opens a file and looks for a specified line.
     * The line is added to the file if it was not there.
     */
    public static void ensureFileHasLine(String fileName, String lineWanted) throws IOException {

        // read the file to determine whether or not the line is already there
        BufferedReader in = new BufferedReader(new FileReader(fileName));
        String line;
        boolean found = false;

        line = in.readLine();
        while (line != null && !found) {
            found = line.equals(lineWanted);
            line = in.readLine();
        }
        in.close();

        if (!found) {
            // the line has not been found: let's add it to the file
            PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName, true)));
            out.println(lineWanted);
            out.close();
        }
    }

    /**
     * Returns the files with a given extension from a specified directory.
     * @param directory the directory to explore (the subdirectories are not explored)
     * @param extension the extension of the files to keep
     */
    public static File[] getFilesWithExtension(String directory, String extension) {

        File directoryFile = new File(directory);
        java.io.FileFilter fileFilter = new ExtensionFileFilter(extension);
        File[] files = directoryFile.listFiles(fileFilter);
        return files;
    }

    /**
     * Returns the name of a file without its extension.
     * The path is not included.
     * @param file a file
     * @return name of this file, without the extension
     */
    public static String getFileNameWithoutExtension(File file) {
        String name = file.getName();
        int dotIndex = name.lastIndexOf('.');

        if (dotIndex != -1) {
            name = name.substring(0, dotIndex);
        }

        return name;
    }

    /**
     * Copies a directory and all its content into another directory.
     * @param from The source directory.
     * @param to The destination directory (will be created if necessary).
     * @throws IOException If an error occurs.
     */
    public static void copyDirectory(String from, String to) throws IOException {
        new File(to).mkdir();
        Files.walkFileTree(Paths.get(from), new CopyFileVisitor(Paths.get(to)));
    }

    /**
     * Deletes a file or a directory with all its content.
     * Does nothing if the file or directory does not exist.
     * @param fileName File or directory to delete.
     */
    public static void deleteRecursive(String fileName) throws IOException {

        File file = new File(fileName);
        if (file.exists()) {
            if (file.isDirectory()) {
                Files.walkFileTree(Paths.get(fileName), new DeleteFileVisitor());
            }
            else {
                file.delete();
            }
        }
    }

    /**
     * Deletes a directory and all its content.
     * Does nothing if the directory does not exist.
     * @param directory Directory to delete.
     */
    public static void deleteDirectory(String directory) throws IOException {
        if (new File(directory).exists()) {
            Files.walkFileTree(Paths.get(directory), new DeleteFileVisitor());
        }
    }

    /**
     * Renames a directory.
     * @param oldName The source directory.
     * @param newName The new name.
     *
     * The new name should not already exist.
     *
     * @throws IOException If an error occurs.
     */
    public static void renameDirectory(String oldName, String newName) throws IOException {

        File newDirectory = new File(newName);
        if (newDirectory.exists()) {
            if (newDirectory.isDirectory()) {
                deleteDirectory(newName);
            }
            else {
                newDirectory.delete();
            }
        }
        new File(oldName).renameTo(newDirectory);
    }

    private static class CopyFileVisitor extends SimpleFileVisitor<Path> {
        private final Path targetPath;
        private Path sourcePath = null;
        public CopyFileVisitor(Path targetPath) {
            this.targetPath = targetPath;
        }

        @Override
        public FileVisitResult preVisitDirectory(final Path dir,
                final BasicFileAttributes attrs) throws IOException {
            if (sourcePath == null) {
                sourcePath = dir;
            } else {
                Files.createDirectories(targetPath.resolve(sourcePath
                            .relativize(dir)));
            }
            return FileVisitResult.CONTINUE;
        }

        @Override
        public FileVisitResult visitFile(final Path file,
                final BasicFileAttributes attrs) throws IOException {
            Files.copy(file,
                    targetPath.resolve(sourcePath.relativize(file)));
            return FileVisitResult.CONTINUE;
        }
    }

    private static class DeleteFileVisitor extends SimpleFileVisitor<Path> {

        @Override
        public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
            Files.delete(file);
            return FileVisitResult.CONTINUE;
        }

        @Override
        public FileVisitResult visitFileFailed(Path file, IOException exc) throws IOException {
            Files.delete(file);
            return FileVisitResult.CONTINUE;
        }

        @Override
        public FileVisitResult postVisitDirectory(Path dir, IOException exc) throws IOException {
            if (exc == null) {
                Files.delete(dir);
                return FileVisitResult.CONTINUE;
            }
            else {
                throw exc;
            }
        }
    }

    /**
     * Extracts the content of a zip file into a directory.
     * @param zipFile Path of the zip file to extract, relative to the
     * quest editor directory.
     * @param destinationPath Where to extract the zip.
     * @throws IOException If the extraction failed.
     */
    public static void extractZipFile(String zipFile, String destinationPath)
            throws IOException {

        InputStream input = FileTools.class.getResourceAsStream("/" + zipFile);
        if (input == null) {
            throw new IOException("Cannot open zip file '" + zipFile + "'");
        }
        ZipInputStream zipStream = new ZipInputStream(input);
        ZipEntry entry;

        int bufferSize = 4096;
        while ((entry = zipStream.getNextEntry()) != null) {

            File destinationFile = new File(destinationPath, entry.getName());

            // Create the parent directories needed.
            destinationFile.getParentFile().mkdirs();

            if (!entry.isDirectory()) {
                int bytesRead;
                byte data[] = new byte[bufferSize];
                BufferedOutputStream output = new BufferedOutputStream(
                        new FileOutputStream(destinationFile));
                while ((bytesRead = zipStream.read(data, 0, bufferSize)) != -1) {
                    output.write(data, 0, bytesRead);
                }
                output.flush();
                output.close();
            }
        }
        zipStream.close();
    }
}

