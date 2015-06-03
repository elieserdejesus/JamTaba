#include "JamRoom.h"
#include <QSet>
#include <QMap>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include "../ninjam/Service.h"
#include "../ninjam/Server.h"




using namespace Login;


QMap<QString, QString> AbstractPeer::countriesMap = {
    {"AF",	"Afghanistan"},
    {"AX",	"Åland Islands"},
    {"AL",	"Albania"},
    {"DZ",	"Algeria"},
    {"AS",	"American Samoa"},
    {"AD",	"Andorra"},
    {"AO",	"Angola"},
    {"AI",	"Anguilla"},
    {"AQ",	"Antarctica"},
    {"AG",	"Antigua and Barbuda"},
    {"AR",	"Argentina"},
    {"AM",	"Armenia"},
    {"AW",	"Aruba"},
    {"AU",	"Australia"},
    {"AT",	"Austria"},
    {"AZ",	"Azerbaijan"},
    {"BS",	"Bahamas"},
    {"BH",	"Bahrain"},
    {"BD",	"Bangladesh"},
    {"BB",	"Barbados"},
    {"BY",	"Belarus"},
    {"BE",	"Belgium"},
    {"BZ",	"Belize"},
    {"BJ",	"Benin"},
    {"BM",	"Bermuda"},
    {"BT",	"Bhutan"},
    {"BO",	"Bolivia"},
    {"BA",	"Bosnia and Herzegovina"},
    {"BW",	"Botswana"},
    {"BV",	"Bouvet Island"},
    {"BR",	"Brazil"},
    {"IO",	"British Indian Ocean Territory"},
    {"BN",	"Brunei Darussalam"},
    {"BG",	"Bulgaria"},
    {"BF",	"Burkina Faso"},
    {"BI",	"Burundi"},
    {"KH",	"Cambodia"},
    {"CM",	"Cameroon"},
    {"CA",	"Canada"},
    {"CV",	"Cape Verde"},
    {"KY",	"Cayman Islands"},
    {"CF",	"Central African Republic"},
    {"TD",	"Chad"},
    {"CL",	"Chile"},
    {"CN",	"China"},
    {"CX",	"Christmas Island"},
    {"CC",	"Cocos (Keeling) Islands"},
    {"CO",	"Colombia"},
    {"KM",	"Comoros"},
    {"CG",	"Congo"},
    {"CD",	"Congo, the Democratic Republic of the"},
    {"CK",	"Cook Islands"},
    {"CR",	"Costa Rica"},
    {"CI",	"Côte D'Ivoire"},
    {"HR",	"Croatia"},
    {"CU",	"Cuba"},
    {"CY",	"Cyprus"},
    {"CZ",	"Czech Republic"},
    {"DK",	"Denmark"},
    {"DJ",	"Djibouti"},
    {"DM",	"Dominica"},
    {"DO",	"Dominican Republic"},
    {"EC",	"Ecuador"},
    {"EG",	"Egypt"},
    {"SV",	"El Salvador"},
    {"GQ",	"Equatorial Guinea"},
    {"ER",	"Eritrea"},
    {"EE",	"Estonia"},
    {"ET",	"Ethiopia"},
    {"FK",	"Falkland Islands (Malvinas)"},
    {"FO",	"Faroe Islands"},
    {"FJ",	"Fiji"},
    {"FI",	"Finland"},
    {"FR",	"France"},
    {"GF",	"French Guiana"},
    {"PF",	"French Polynesia"},
    {"TF",	"French Southern Territories"},
    {"GA",	"Gabon"},
    {"GM",	"Gambia"},
    {"GE",	"Georgia"},
    {"DE",	"Germany"},
    {"GH",	"Ghana"},
    {"GI",	"Gibraltar"},
    {"GR",	"Greece"},
    {"GL",	"Greenland"},
    {"GD",	"Grenada"},
    {"GP",	"Guadeloupe"},
    {"GU",	"Guam"},
    {"GT",	"Guatemala"},
    {"GG",	"Guernsey"},
    {"GN",	"Guinea"},
    {"GW",	"Guinea-Bissau"},
    {"GY",	"Guyana"},
    {"HT",	"Haiti"},
    {"HM",	"Heard Island and Mcdonald Islands"},
    {"VA",	"Holy See (Vatican City State)"},
    {"HN",	"Honduras"},
    {"HK",	"Hong Kong"},
    {"HU",	"Hungary"},
    {"IS",	"Iceland"},
    {"IN",	"India"},
    {"ID",	"Indonesia"},
    {"IR",	"Iran, Islamic Republic of"},
    {"IQ",	"Iraq"},
    {"IE",	"Ireland"},
    {"IM",	"Isle of Man"},
    {"IL",	"Israel"},
    {"IT",	"Italy"},
    {"JM",	"Jamaica"},
    {"JP",	"Japan"},
    {"JE",	"Jersey"},
    {"JO",	"Jordan"},
    {"KZ",	"Kazakhstan"},
    {"KE",	"KENYA"},
    {"KI",	"Kiribati"},
    {"KP",	"Korea, Democratic People's Republic of"},
    {"KR",	"Korea, Republic of"},
    {"KW",	"Kuwait"},
    {"KG",	"Kyrgyzstan"},
    {"LA",	"Lao People's Democratic Republic"},
    {"LV",	"Latvia"},
    {"LB",	"Lebanon"},
    {"LS",	"Lesotho"},
    {"LR",	"Liberia"},
    {"LY",	"Libyan Arab Jamahiriya"},
    {"LI",	"Liechtenstein"},
    {"LT",	"Lithuania"},
    {"LU",	"Luxembourg"},
    {"MO",	"Macao"},
    {"MK",	"Macedonia, the Former Yugoslav Republic of"},
    {"MG",	"Madagascar"},
    {"MW",	"Malawi"},
    {"MY",	"Malaysia"},
    {"MV",	"Maldives"},
    {"ML",	"Mali"},
    {"MT",	"Malta"},
    {"MH",	"Marshall Islands"},
    {"MQ",	"Martinique"},
    {"MR",	"Mauritania"},
    {"MU",	"Mauritius"},
    {"YT",	"Mayotte"},
    {"MX",	"Mexico"},
    {"FM",	"Micronesia, Federated States of"},
    {"MD",	"Moldova, Republic of"},
    {"MC",	"Monaco"},
    {"MN",	"Mongolia"},
    {"ME",	"Montenegro"},
    {"MS",	"Montserrat"},
    {"MA",	"Morocco"},
    {"MZ",	"Mozambique"},
    {"MM",	"Myanmar"},
    {"NA",	"Namibia"},
    {"NR",	"Nauru"},
    {"NP",	"Nepal"},
    {"NL",	"Netherlands"},
    {"AN",	"Netherlands Antilles"},
    {"NC",	"New Caledonia"},
    {"NZ",	"New Zealand"},
    {"NI",	"Nicaragua"},
    {"NE",	"Niger"},
    {"NG",	"Nigeria"},
    {"NU",	"Niue"},
    {"NF",	"Norfolk Island"},
    {"MP",	"Northern Mariana Islands"},
    {"NO",	"Norway"},
    {"OM",	"Oman"},
    {"PK",	"Pakistan"},
    {"PW",	"Palau"},
    {"PS",	"Palestinian Territory, Occupied"},
    {"PA",	"Panama"},
    {"PG",	"Papua New Guinea"},
    {"PY",	"Paraguay"},
    {"PE",	"Peru"},
    {"PH",	"Philippines"},
    {"PN",	"Pitcairn"},
    {"PL",	"Poland"},
    {"PT",	"Portugal"},
    {"PR",	"Puerto Rico"},
    {"QA",	"Qatar"},
    {"RE",	"Réunion"},
    {"RO",	"Romania"},
    {"RU",	"Russian"},
    {"RW",	"Rwanda"},
    {"SH",	"Saint Helena"},
    {"KN",	"Saint Kitts and Nevis"},
    {"LC",	"Saint Lucia"},
    {"PM",	"Saint Pierre and Miquelon"},
    {"VC",	"Saint Vincent and the Grenadines"},
    {"WS",	"Samoa"},
    {"SM",	"San Marino"},
    {"ST",	"Sao Tome and Principe"},
    {"SA",	"Saudi Arabia"},
    {"SN",	"Senegal"},
    {"RS",	"Serbia"},
    {"SC",	"Seychelles"},
    {"SL",	"Sierra Leone"},
    {"SG",	"Singapore"},
    {"SK",	"Slovakia"},
    {"SI",	"Slovenia"},
    {"SB",	"Solomon Islands"},
    {"SO",	"Somalia"},
    {"ZA",	"South Africa"},
    {"GS",	"South Georgia and the South Sandwich Islands"},
    {"ES",	"Spain"},
    {"LK",	"Sri Lanka"},
    {"SD",	"Sudan"},
    {"SR",	"Suriname"},
    {"SJ",	"Svalbard and Jan Mayen"},
    {"SZ",	"Swaziland"},
    {"SE",	"Sweden"},
    {"CH",	"Switzerland"},
    {"SY",	"Syrian Arab Republic"},
    {"TW",	"Taiwan, Province of China"},
    {"TJ",	"Tajikistan"},
    {"TZ",	"Tanzania, United Republic of"},
    {"TH",	"Thailand"},
    {"TL",	"Timor-Leste"},
    {"TG",	"Togo"},
    {"TK",	"Tokelau"},
    {"TO",	"Tonga"},
    {"TT",	"Trinidad and Tobago"},
    {"TN",	"Tunisia"},
    {"TR",	"Turkey"},
    {"TM",	"Turkmenistan"},
    {"TC",	"Turks and Caicos Islands"},
    {"TV",	"Tuvalu"},
    {"UG",	"Uganda"},
    {"UA",	"Ukraine"},
    {"AE",	"United Arab Emirates"},
    {"GB",	"United Kingdom"},
    {"US",	"United States"},
    {"UM",	"United States Minor Outlying Islands"},
    {"UY",	"Uruguay"},
    {"UZ",	"Uzbekistan"},
    {"VU",	"Vanuatu"},
    {"VA",	"Vatican City State"},
    {"VE",	"Venezuela"},
    {"VN",	"Viet Nam"},
    {"VG",	"Virgin Islands, British"},
    {"VI",	"Virgin Islands, U.S."},
    {"WF",	"Wallis and Futuna"},
    {"EH",	"Western Sahara"},
    {"YE",	"Yemen"},
    {"CD",	"Zaire"},
    {"ZM",	"Zambia"},
    {"ZW",	"Zimbabwe"}
};

QMap<long long, std::shared_ptr<AbstractPeer>> AbstractPeer::peers;// = QMap<long long, std::shared_ptr<AbstractPeer>>();

AbstractPeer::AbstractPeer(long long ID)
    :id(ID), countryCode("unknown")
{

}

AbstractPeer::~AbstractPeer(){
    //qDebug() << "AbstractPeer destructor " << getName();
}

AbstractPeer* AbstractPeer::getByIP(QString ip){
    foreach (auto peer, AbstractPeer::peers.values()) {
        if(peer->getIP() == ip){
            return &(*peer);
        }
    }
    return nullptr;
}

//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
NinjamPeer::NinjamPeer(long long ID)
    :AbstractPeer(ID)
{

}



void NinjamPeer::updateFromJson(QJsonObject json){
    if(this->name.isNull() || this->name.isEmpty()){
        this->name = json["name"].toString();
        this->ip = json["ip"].toString();
        this->countryCode = json["countryCode"].toString();
    }
}

bool NinjamPeer::isBot() const
{
    return Ninjam::Service::isBotName(this->name);
}



//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
RealTimePeer::RealTimePeer(long long ID)
    :AbstractPeer(ID)
{

}

void RealTimePeer::updateFromJson(QJsonObject /*json*/){

}

//++++++++++++++++++
//++++++++++++++++++
//++++++++++++++++++

AbstractJamRoom::AbstractJamRoom(long long roomID)
    : id(roomID),
      roomName(""), maxUsers(0)
{
    //qDebug() << "AbstractJamRoom constructor ID:" << id;
}


AbstractJamRoom::~AbstractJamRoom(){
    //qDebug() << "AbstractJamRoom destructor!";
}

bool AbstractJamRoom::containsPeer(long long peerId) const{
    return peers.contains(peerId);
}

void AbstractJamRoom::addPeer(AbstractPeer* peer) {
    if (!peers.contains(peer->getId())) {
        peers.insert(peer->getId(), std::shared_ptr<AbstractPeer>(peer));
        qDebug() << "Adicionou peer " << peer->getName();
    }
}

void AbstractJamRoom::removePeer(AbstractPeer *peer) {
    int removed = peers.remove(peer->getId());
    if(removed > 0){
        qDebug() << "Peer removido: " << peer->getName();
    }
}

//void AbstractJamRoom::refreshPeerList(const QMap<long long, AbstractPeer *> &peersInServer) {
//    //verifica peers adicionados
//    QSet<long long> serverKeys = QSet<long long>::fromList( peersInServer.keys());
//    foreach (long long serverKey , serverKeys) {
//        if (!peers.contains(serverKey)) {
//            addPeer(peersInServer[serverKey]);
//        }
//    }
//    //verifica peers que devem ser removidos
//    QList<long long> keysToRemove;
//    foreach (long long key , peers.keys()) {
//        if (!serverKeys.contains(key)) {
//            //se a chave não esta mais na lista de peers do servidor então o peer saiu da sala
//            keysToRemove.append(key);
//        }
//    }
//    foreach (long long key , keysToRemove) {
//        removePeer(peers[key]);
//    }
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool AbstractJamRoom::isFull() const{
    return peers.size() >= getMaxUsers();
}

QString AbstractJamRoom::getName() const{
    return roomName;
}

QList<AbstractPeer*> AbstractJamRoom::getPeers() const{
    QList<AbstractPeer*> list;
    for(std::shared_ptr<AbstractPeer> p : peers.values()){
        list.append(&*p);
    }
    return list;
}


int AbstractJamRoom::getPeersCount() const{
    return peers.size();
}

bool AbstractJamRoom::hasPeers() const{
    return peers.size() > 0;
}





const AbstractPeer *AbstractJamRoom::getPeer(long long peerID) const{
    return peers[peerID].get();
}

bool AbstractJamRoom::isEmpty() const{
    return peers.isEmpty();
}

bool Login::jamRoomLessThan(AbstractJamRoom* r1, AbstractJamRoom* r2)
{
     return r1->getPeersCount() > r2->getPeersCount();
}

//+++++++++++++++++++++++++++++++++++++++++++++++

RealTimeRoom::RealTimeRoom(long long id)
    :AbstractJamRoom(id)
{
    // qDebug() << "RealTimeRoom constructor";
}

RealTimeRoom::~RealTimeRoom()
{
    // qDebug() << "RealTimeRoom destructor!";
}

bool RealTimeRoom::updateFromJson(QJsonObject json){
    if(this->roomName.isNull() || this->roomName.isEmpty()){
        this->roomName = json["name"].toString();
        this->maxUsers = json["maxUsers"].toInt();
    }
    return false;
}


/***
     *
     * @param localPeer - Skip local peer
     * @return the reachable peers list skipping local peer
     */
QList<RealTimePeer*> RealTimeRoom::getReachablePeers(AbstractPeer* localPeer) const{
    QList<AbstractPeer*> allPeers = getPeers();
    QList<RealTimePeer*> reachablePeers;
    if(localPeer == nullptr){//inclui o local peer na lista
        foreach (AbstractPeer* peer , allPeers) {
            if (((RealTimePeer*)peer)->isReachable()) {
                reachablePeers.append((RealTimePeer*)peer);
            }
        }
    }
    else{//exclui o peer local da lista
        foreach (AbstractPeer* peer , allPeers) {
            if (((RealTimePeer*)peer)->isReachable() && peer != localPeer) {
                reachablePeers.append((RealTimePeer*)peer);
            }
        }
    }
    return reachablePeers;
}

QList<RealTimePeer *> RealTimeRoom::getReachablePeers() const{
    return getReachablePeers(nullptr);
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++             NINJAM  ROOM         ++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QMap<QString, NinjamRoom*> NinjamRoom::ninjamRooms;

NinjamRoom::NinjamRoom(long long ID)
    :AbstractJamRoom(ID), containBotPeer(false), ninjamServer(nullptr)
{

}

//this constructor is used just to test the localhost ninjam server
NinjamRoom::NinjamRoom(QString host, int port, int maxUsers)
    :AbstractJamRoom(1), ninjamServer(nullptr)
{
    this->hostName = host;
    this->hostPort = port;
    this->maxUsers = maxUsers;
    this->streamLink = "";

}

NinjamRoom::~NinjamRoom(){
    ninjamRooms.remove(getMapKey());
}

QString NinjamRoom::getMapKey() const{
    return buildMapKey(hostName, hostPort);
}

bool NinjamRoom::updateFromJson(QJsonObject json){
    if(this->hostName.isEmpty() || this->hostName.isNull()){//not initialized
        this->hostName = json["name"].toString();
        this->hostPort = json["port"].toInt();
        this->maxUsers = json["maxUsers"].toInt();
        this->streamLink = json["streamUrl"].toString();

        ninjamRooms.insert(getMapKey(), this);
    }

    //insert new users and removed the olds
    bool changed = false;
    QJsonArray usersArray = json["users"].toArray();
    QSet<long long> idsInServer;
    for (int i = 0; i < usersArray.size(); ++i) {//check for new users
        QJsonObject userObject = usersArray[i].toObject();
        long long userID = userObject.value("id").toVariant().toLongLong();
        idsInServer.insert(userID);
        if(!peers.contains(userID)){
            AbstractPeer* newPeer = dynamic_cast<AbstractPeer*>(new NinjamPeer(userID));
            peers.insert(userID, std::shared_ptr<AbstractPeer>(newPeer));
            newPeer->updateFromJson(userObject);
            if(newPeer->isBot()){
                this->containBotPeer = true;
            }
            changed = true;
        }
    }

    //check for removed users
    for(long long userID : peers.keys()){
        if(!idsInServer.contains(userID)){
            peers.remove(userID);
            changed = true;
        }
    }


    return changed;
}


bool NinjamRoom::hasStreamLink() const{
    return !streamLink.isNull() && !streamLink.isEmpty();
}

QString NinjamRoom::getStreamLink() const{
    if(!hasStreamLink()){
        return "";
    }
    return streamLink;
}

bool NinjamRoom::isEmpty() const{
    if(containBot()){
        return getPeersCount() - 1 <= 0;
    }
    return AbstractJamRoom::isEmpty();
}


QString NinjamRoom::getName() const{
    return hostName + ":" + QString::number(hostPort);
}

QString NinjamRoom::buildMapKey(QString hostName, int hostPort){
    return hostName + ":" + QString::number(hostPort);
}

NinjamRoom* NinjamRoom::getNinjamRoom(const Ninjam::Server &server)
{
    QString key = buildMapKey(server.getHostName(), server.getPort());
    if(ninjamRooms.contains(key)){
        NinjamRoom* room = ninjamRooms[key];
        room->setNinjamServer((Ninjam::Server*)&server);
        return room;
    }
    return nullptr;
}

//int NinjamRoom::getPeersCount() const{
//    if(containBot()){
//        return AbstractJamRoom::getPeersCount() - 1;
//    }
//    return AbstractJamRoom::getPeersCount();
//}

