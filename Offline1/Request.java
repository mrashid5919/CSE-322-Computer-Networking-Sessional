public class Request {
    private int ID;
    private String requester;
    private String description;

    public Request(int ID, String requester, String description) {
        this.ID = ID;
        this.requester = requester;
        this.description = description;
    }

    public void acceptRequest(String granter, String filepath){

    }

    public int getID(){
        return ID;
    }
}
