# MusicStreamer
Share your favourite music with friends right now!

# Api
````
{
	"method": ...,//one of {INIT, JOIN, LEAVE, ROOMS, UPLOAD}
	// fields for given method
}
````
Each of method has different body request and reponse: 
* `INIT`: requires field `name` with user name. Auth can be done only once.
* `JOIN`: requires field `roomName` - swiches currently used by user room. When given room does not exist, server will create it.
When after room change there will be no more clients in given room, it will be destroyed.
* `LEAVE`: moves client outside of the room.
* `UPLOAD`: sending to server request for sending file to it.
