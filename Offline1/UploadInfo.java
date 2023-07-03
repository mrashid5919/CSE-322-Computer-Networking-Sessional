public class UploadInfo {
    private String uploader;
    private String filepath;

    public String getUploader() {
        return uploader;
    }

    public String getFilepath() {
        return filepath;
    }

    public UploadInfo(String uploader, String filepath) {
        this.uploader = uploader;
        this.filepath = filepath;
    }
}
