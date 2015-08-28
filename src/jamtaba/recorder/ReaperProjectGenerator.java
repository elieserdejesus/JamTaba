package jamtaba.recorder;

import jamtaba.recorder.metadata.JamMetadata;
import jamtaba.recorder.metadata.JamMetadataPaths;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.List;
import java.util.UUID;

/**
 * @author elieser
 */
public class ReaperProjectGenerator implements JamMetadataWriter {

    //TODO a posição não é tão simples como estou fazendo, preciso considerar o intervalo em que o item foi adicionado
    //TODO está gerando sempre compassos em 4/4
    @Override
    public void write(JamMetadata metadata) throws Exception {
        StringBuilder stringBuffer = new StringBuilder();
        stringBuffer.append("<REAPER_PROJECT 0.1 \"4.731\" 1416709867").append(System.lineSeparator());
        stringBuffer.append("  RECORD_PATH \"" + metadata.getJamAudioDirectory() + "\" \"\"").append(System.lineSeparator());
        stringBuffer.append("  SAMPLERATE " + metadata.getSampleRate() + "  0 0").append(System.lineSeparator());
        stringBuffer.append("  TEMPO " + metadata.getBPM() + " 4 4").append(System.lineSeparator());

        
        //generate tracks
        for (String userName : metadata.getUserNames()) {
            for (String channelName : metadata.getUserChannels(userName)) {
                String trackName = buildTrackName(userName, channelName);
                String trackGUID = UUID.randomUUID().toString();
                stringBuffer.append("  <TRACK {"+ trackGUID +"}").append(System.lineSeparator());
                stringBuffer.append("    NAME \"" + trackName + "\"").append(System.lineSeparator());
                stringBuffer.append("    TRACKID {" + trackGUID +"}").append(System.lineSeparator());
                List<JamMetadata.JamAudioChunk> channelAudioFiles = metadata.getChannelAudioFiles(userName, channelName);
                int part = 1;
                for (JamMetadata.JamAudioChunk audioChunk : channelAudioFiles) {
                    double position = audioChunk.getIntervalIndex() * metadata.getIntervalsLenght();
                    String filePath = audioChunk.getAudioFile().toString();
                    
                    stringBuffer.append("    <ITEM").append(System.lineSeparator());
                    stringBuffer.append("      POSITION " + position).append(System.lineSeparator());
                    stringBuffer.append("      LENGTH " + metadata.getIntervalsLenght()).append(System.lineSeparator());
                    stringBuffer.append("      FADEIN 1 0.01 0 1 0 0").append(System.lineSeparator());
                    stringBuffer.append("      FADEOUT 1 0.01 0 1 0 0").append(System.lineSeparator());
                    stringBuffer.append("      IID " + part).append(System.lineSeparator());
                    stringBuffer.append("      IGUID {"+ UUID.randomUUID().toString() +"}").append(System.lineSeparator());
                    stringBuffer.append("      NAME \"" + audioChunk.getAudioFile().getName(audioChunk.getAudioFile().getNameCount()-1) + "\"").append(System.lineSeparator());
                    stringBuffer.append("      GUID {"+ trackGUID +"}").append(System.lineSeparator());
                    stringBuffer.append("      <SOURCE VORBIS").append(System.lineSeparator());
                    stringBuffer.append("        FILE \"" + filePath + "\"").append(System.lineSeparator());
                    stringBuffer.append("      >").append(System.lineSeparator());//close SOURCE VORBIS
                    stringBuffer.append("    >").append(System.lineSeparator());//close item
                    part++;
                }
                stringBuffer.append("  >").append(System.lineSeparator());//close track
            }
        }
        stringBuffer.append(">");//close the root tag
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //save
        FileSystem fs = FileSystems.getDefault();
        Path dirPath = metadata.getRecordingBaseDirectory().resolve(metadata.getJamDirectory());
        if(!Files.exists(dirPath)){
            Files.createDirectories(dirPath);
        }
        Path projectFilePath = dirPath.resolve("Reaper project.rpp");
        FileChannel fileChannel = FileChannel.open( projectFilePath, StandardOpenOption.WRITE, StandardOpenOption.CREATE);
        byte bytes[] = stringBuffer.toString().getBytes("utf-8");
        fileChannel.write(ByteBuffer.wrap(bytes));
        fileChannel.close();
    }

    private static String buildTrackName(String userName, String channelName) {
        return userName + " (" + channelName + ")";
    }

    public static void main(String args[]) throws Exception {
        
        JamMetadataWriter writer = new ReaperProjectGenerator();
        String jamName = "teste";
        String baseDirectory = "Jamtaba JAMS";
        String jamAudioDirectory = "audio";
        int jamSampleRate = 44100;
        FileSystem fs = FileSystems.getDefault();
        JamMetadataPaths paths = new JamMetadataPaths(fs.getPath(baseDirectory), fs.getPath(jamName), fs.getPath(jamAudioDirectory));
        JamMetadata metadata = new JamMetadata(91, 16, jamName, jamSampleRate, paths);
        for (int user = 0; user < 2; user++) {
            for (int channel = 0; channel < 2; channel++) {
                for (int file = 0; file < 3; file++) {
                    metadata.addAudioFile("User " + user, "Channel " + channel, fs.getPath("part " + file + ".ogg"), file);
                }
            }
        }
        writer.write(metadata);
    }

}
