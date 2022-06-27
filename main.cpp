#include <iostream>
#include <map>
#include <functional>

#include "net.h"
#include "websocket.h"

using namespace std;

class W2MCH
{
public:
    W2MCH()
    {

    }

    std::string title="w2 MultiChannel";
    std::string h1="W2 LTE Multi channnel";
    std::string selected_id="all";
    int sens_nmb=4;

    std::map<int,int> sensor_ids{{100,1},{200,2},{300,3},{400,0}};
    std::map<int,int>::iterator sensor_it;

    std::string generateMain()
    {
        std::string s="<!DOCTYPE html> \
                <html lang=\"en\"> \
                <head> \
                <title>"+title+"</title> \
                <meta charset=\"utf-8\"> \
                <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
                <link rel=\"stylesheet\" href=\"bootstrap.min.css\"> \
                <script src=\"jquery.js\"></script> \
                <script src=\"bootstrap.min.js\"></script> \
                </head> ";

                s+="<body>"
                   "<div class=\"jumbotron text-center\"> \
                <h1>" + h1 + "</h1> <p id=\"clock\"> - </p> \
                <select id=\"sensors\">";
        s+="<option value=\"all\">all</option>";
        std::string sel="";
        for(auto& sens: sensor_ids)
        {
            if(std::to_string(sens.first) == selected_id)
            {
                sel="selected";
            }
            else
            {
                sel="";
            }
            s+= "<option value=\""+std::to_string(sens.first)+"\" "+sel+ ">"+std::to_string(sens.first)+"</option>";
        }



        s+="</select> \
                </div>";

                s+="<div class = \"container\">";
        s+="<div class=\"row row-no-gutters\">";
        s+="<div class=\"col-sm-1\">"
           "<h3>ID</h3>"
           "</div>"
           "<div class=\"col-sm-2\">"
           " <h3>Controll</h3>"
           " </div>"
           "<div class=\"col-sm-1\">"
           "<h3>Status</h3>"
           " </div>"
           "<div class=\"col-sm-2\">"
           "<h3>Start</h3>"
           "</div>"
           "<div class=\"col-sm-2\">"
           "<h3>Stop</h3>"
           " </div>"
           "<div class=\"col-sm-4\">"
           "<h3>Initial DT</h3>"
           " </div>";
        s+="</div>"; //row
        s+="<script src=\"w2mch.js\"></script>";
        std::string status="---",start="---",stop="---";
        for(auto& sens: sensor_ids)
        {
            if(selected_id!="all" && selected_id!=std::to_string(sens.first))
                continue;
            s+="<div class=\"row row-no-gutters\"> \
                    <div class=\"col-sm-1\"> "
                                " <p id=\"ID\">"
                    +std::to_string(sens.first)+"</p> \
                    </div>\
                    <div class=\"col-sm-2\"> \
                    <p> \
                    <button id=\"get"+std::to_string(sens.first)+"\""+"type=\"button\" class=\"btn btn-primary\" onclick='onGetClick(\""
                    +std::to_string(sens.first)+
                    "\")'>Get</button> \
                    <button id="+"\"live"+std::to_string(sens.first)+"\""+"type=\"button\" class=\"btn btn-primary\" onclick='onLiveClick(\""
                    +std::to_string(sens.first)+
                    "\")'>Live</button>";
            s+="</div> <div class=\"col-sm-1\"> \
                    <p id=\"status"+std::to_string(sens.first)+"\">---</p> </div>";
            s+="<div class=\"col-sm-2\"> \
                    <p> <input id=\"startdt"+std::to_string(sens.first)+"\">"
                                                                        "</input>"
                                                                        "</p> \
                    </div> \
                    <div class=\"col-sm-2\"> \
                    <p> <input id=\"stopdt"+std::to_string(sens.first)+"\">"
                                                                       "</input>"
                                                                       "</p> \
                    </div> \
                    <div class=\"col-sm-4\"> \
                    <p> <id=\"init"+std::to_string(sens.first)+"\">18.05.22 18:17:52"
                                                               "</p> \
                    </div>";
                    //s+="<script>document.getElementById(startdt"+std::to_string(sens.first)+").setAttribute('size', '12'); </script>";
                    //s+="<script>document.getElementById(stopdt"+std::to_string(sens.first)+").setAttribute('size', '12'); </script>";
                    s+="</div>";
        }
        s+="</div>"; //container
        s+="</body>"
           "</html>";
        return s;
    }
}w2mch;

class W2Mch_WebSocket : public awl::Core::Thread
{
public:

    W2MCH* w2mch;

    using WSServer = websocket::WSServer<W2Mch_WebSocket>;
    using WSConn = WSServer::Connection;

    void onstart()
    {
        if(wsserver.init("127.0.0.1",18100))
        {
            std::cout << "Websocket INIT OK" << std::endl;
        }
        else
        {
            std::cout << "Websocket init error" << std::endl;
        }
    }

    void run()
    {
        wsserver.poll(this);
        awl::Core::awl_delay_ms(100);
    }

    bool onWSConnect(WSConn& conn, const char* request_uri, const char* host, const char* origin, const char* protocol,
                     const char* extensions, char* resp_protocol, uint32_t resp_protocol_size, char* resp_extensions,
                     uint32_t resp_extensions_size)
    {
        struct sockaddr_in addr;
        conn.getPeername(addr);
        std::cout << "WS Connection from: "  << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;
        std::cout << "request_uri: " << request_uri << std::endl;
        std::cout << "host: " << host << std::endl;
        /*
        if (origin) {
            std::cout << "origin: " << origin << std::endl;
        }
        if (protocol) {
            std::cout << "protocol: " << protocol << std::endl;
        }
        if (extensions) {
            std::cout << "extensions: " << extensions << std::endl;
        }
        */
        return true;
    }

    void onWSClose(WSConn& conn, uint16_t status_code, const char* reason)
    {
        std::cout << "ws close, status_code: " << status_code << ", reason: " << reason << std::endl;
    }


    bool onWSMsg(WSConn& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len)
    {
        //std::cout << "======WS Message======" << std::endl;
        std::string s="";

        std::string sensors;
        if(w2mch->selected_id=="all")
        {
            for(auto& sens: w2mch->sensor_ids)
            {
                sensors+=std::to_string(sens.first)+","+std::to_string(sens.second)+"\r\n";
            }
        }
        else
        {
            int id=std::atoi(w2mch->selected_id.c_str());
            std::map<int,int>::iterator it=w2mch->sensor_ids.find(id);
            if(it!=w2mch->sensor_ids.end())
            {
                sensors=sensors+std::to_string(it->first)+","+std::to_string(it->second)+"\r\n";
            }
        }
        conn.send(websocket::OPCODE_TEXT,(const uint8_t*)sensors.data(),sensors.length());
        return true;
    }

    void onWSSegment(WSConn& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len, uint32_t pl_start_idx,
                     bool fin) {
        std::cout << "WS Error!" << std::endl;
    }

private:
    WSServer wsserver;

};



int main()
{
    cout << "start!" << endl;


    /*
    std::string s="aaaa\r\nbbbb\r\ncccc\r\n\r\n1234";
    awl::StringList slist;
    awl::Core::awl_split(s,"\r\n",slist);

    std::cout << slist.size() << std::endl;

    for(int i=0; i<slist.size(); i++)
    {
        std::cout << slist.at(i) << std::endl;
    }
    return 0;
    */

    awl::Net::SockAddr httpddress( "127.0.0.1",18080);
    awl::Net::HTTPServer http(httpddress,false);

    W2Mch_WebSocket websocket;
    websocket.w2mch = &w2mch;


    http.route("/w2mch", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;

        std::string s_resp= "HTTP/1.0 200 OK\r\n"
                            "Conternt-Type: text/html\r\n"
                            "\r\n";
        s_resp+= w2mch.generateMain();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/html";
        resp.mime_type=awl::Net::mt_Text;

        return resp;
    });

    http.route("/senschng", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{
        std::string s_resp= "HTTP/1.0 200 OK\r\n"
                            "Conternt-Type: text/html\r\n"
                            "\r\n";

        //std::cout << req.path << std::endl;
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/html";
        resp.mime_type=awl::Net::mt_Text;

        w2mch.selected_id=req.body;

        return resp;

    });

    http.route("/bootstrap.min.css", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;
        std::string s_resp="HTTP/1.0 200 OK\r\n"
                           "Conternt-Type: text/css\r\n"
                           "\r\n";

        awl::Core::File f;
        f.open("/home/dato/bootstrap/css/bootstrap.min.css");
        s_resp += f.readAllinString();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/css";
        resp.mime_type=awl::Net::mt_Text;
        return resp;
    });

    http.route("/bootstrap.min.css.map", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;
        std::string s_resp="HTTP/1.0 200 OK\r\n"
                           "Conternt-Type: text/css\r\n"
                           "\r\n";

        awl::Core::File f;
        f.open("/home/dato/bootstrap/css/bootstrap.min.css.map");
        s_resp += f.readAllinString();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/css";
        resp.mime_type=awl::Net::mt_Text;
        return resp;
    });

    http.route("/jquery.min.js", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;

        std::string s_resp="HTTP/1.0 200 OK\r\n"
                           "Conternt-Type: text/js\r\n"
                           "\r\n";

        awl::Core::File f;
        f.open("/home/dato/bootstrap/js/jquery.min.js");
        s_resp+=f.readAllinString();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/js";
        resp.mime_type=awl::Net::mt_Text;
        return resp;
    });

    http.route("/jquery.js", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;

        std::string s_resp="HTTP/1.0 200 OK\r\n"
                           "Conternt-Type: text/js\r\n"
                           "\r\n";

        awl::Core::File f;
        f.open("/home/dato/bootstrap/jquery-3.6.0.js");
        s_resp+=f.readAllinString();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/js";
        resp.mime_type=awl::Net::mt_Text;
        return resp;
    });

    http.route("/bootstrap.min.js", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;

        std::string s_resp="HTTP/1.0 200 OK\r\n"
                           "Conternt-Type: text/js\r\n"
                           "\r\n";
        awl::Core::File f;
        f.open("/home/dato/bootstrap/js/bootstrap.min.js");
        s_resp += f.readAllinString();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/js";
        resp.mime_type=awl::Net::mt_Text;
        return resp;
    });

    http.route("/bootstrap.min.js.map", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;

        std::string s_resp="HTTP/1.0 200 OK\r\n"
                           "Conternt-Type: text/js\r\n"
                           "\r\n";
        awl::Core::File f;
        f.open("/home/dato/bootstrap/js/bootstrap.min.js.map");
        s_resp += f.readAllinString();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/js";
        resp.mime_type=awl::Net::mt_Text;
        return resp;
    });


    http.route("/w2mch.js", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;

        std::string s_resp="HTTP/1.0 200 OK\r\n"
                           "Conternt-Type: text/js\r\n"
                           "\r\n";
        awl::Core::File f;
        f.open("/home/dato/w2_appserver/w2mch.js");
        s_resp += f.readAllinString();
        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        resp.mime="text/js";
        resp.mime_type=awl::Net::mt_Text;
        return resp;
    });


    http.route("/t1.mp3", [](awl::Net::HTTP_Request req)->awl::Net::HTTP_Response{

        //std::cout << req.path << std::endl;

        std::string s_resp= "HTTP/1.0 200 OK\r\n"
                            "Conternt-Type: audio/mpeg\r\n"
                            "\r\n";
        awl::Core::File f;
        f.open("/home/dato/http_test/t1.mp3");

        s_resp += f.readAllinString();

        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        //resp.d_response=f.fdata;
        resp.mime="audio/mpeg";
        resp.mime_type=awl::Net::mt_Text;
        f.close();

        return resp;
    });



    /*
    http.route("/ttt.mp3", [](std::string s)->awl::Net::HTTP_Response{

        std::cout << s << std::endl;

        std::filebuf fb;
        fb.open("/home/dato/http_test/t1.mp3",std::ios::in);
        std::ostream os(&fb);

        os << "HTTP/1.0 200 OK\r\n"
              "Conternt-Type: audio/mpeg\r\n"
              "\r\n";



        awl::Net::HTTP_Response resp;
        resp.s_response=s_resp;
        //resp.d_response=f.fdata;
        resp.mime="audio/mpeg";
        resp.mime_type=awl::Net::mt_Text;
        f.close();

        return resp;
    });
    */


    websocket.start(true);
    http.start(true);

    while(1)
    {
        awl::Core::awl_delay_ms(1000);
    }


    return 0;
}
