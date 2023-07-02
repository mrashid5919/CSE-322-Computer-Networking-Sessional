import java.util.ArrayList;

public class Request {
    private int ID;
    private String requester;
    private String description;
    private ArrayList<UploadInfo> uploads;

    public Request(int ID, String requester, String description) {
        this.ID = ID;
        this.requester = requester;
        this.description = description;
        uploads=new ArrayList<>();
    }

    public void acceptRequest(String granter, String filepath){
        UploadInfo x=new UploadInfo(granter,filepath);
        uploads.add(x);
    }

    public int getID(){
        return ID;
    }

    public String getRequester(){
        return requester;
    }

    public String getDescription(){
        return description;
    }

    public ArrayList<UploadInfo> getUploads(){
        return uploads;
    }
}
