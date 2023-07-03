import java.util.ArrayList;
import java.util.Arrays;

public class Request {
    private int ID;
    private String requester;
    private String description;
    private ArrayList<UploadInfo> uploads;
    private ArrayList<String> unseen;
    private int seen;

    public Request(int ID, String requester, String description) {
        this.ID = ID;
        this.requester = requester;
        this.description = description;
        uploads=new ArrayList<>();
        unseen=new ArrayList<>();
        String[] all=Server.allUsers();
        unseen.addAll(Arrays.asList(all));
        unseen.remove(requester);
        seen=0;
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

    public ArrayList<String> getunSeen(){
        return unseen;
    }

    public void setUnseen(String s){
        unseen.remove(s);
    }

    public void setSeen(int seen){
        this.seen=seen;
    }

    public int getSeen(){
        return this.seen;
    }
}
