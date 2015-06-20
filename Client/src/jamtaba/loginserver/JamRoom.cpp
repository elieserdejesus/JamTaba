#include "JamRoom.h"
#include <QSet>
#include <QMap>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include "../ninjam/Service.h"
#include "../ninjam/Server.h"
#include <cstring>

using namespace Login;


 static QMap<QString, QString> getCountries() {
    QMap<QString,QString> map;
     map.insert("AF",	"Afghanistan");
    map.insert("AX",	"Åland Islands");
    map.insert("AL",	"Albania");
    map.insert("DZ",	"Algeria");
    map.insert("AS",	"American Samoa");
    map.insert("AD",	"Andorra");
    map.insert("AO",	"Angola");
    map.insert("AI",	"Anguilla");
    map.insert("AQ",	"Antarctica");
    map.insert("AG",	"Antigua and Barbuda");
    map.insert("AR",	"Argentina");
    map.insert("AM",	"Armenia");
    map.insert("AW",	"Aruba");
    map.insert("AU",	"Australia");
    map.insert("AT",	"Austria");
    map.insert("AZ",	"Azerbaijan");
    map.insert("BS",	"Bahamas");
    map.insert("BH",	"Bahrain");
    map.insert("BD",	"Bangladesh");
    map.insert("BB",	"Barbados");
    map.insert("BY",	"Belarus");
    map.insert("BE",	"Belgium");
    map.insert("BZ",	"Belize");
    map.insert("BJ",	"Benin");
    map.insert("BM",	"Bermuda");
    map.insert("BT",	"Bhutan");
    map.insert("BO",	"Bolivia");
    map.insert("BA",	"Bosnia and Herzegovina");
    map.insert("BW",	"Botswana");
    map.insert("BV",	"Bouvet Island");
    map.insert("BR",	"Brazil");
    map.insert("IO",	"British Indian Ocean Territory");
    map.insert("BN",	"Brunei Darussalam");
    map.insert("BG",	"Bulgaria");
    map.insert("BF",	"Burkina Faso");
    map.insert("BI",	"Burundi");
    map.insert("KH",	"Cambodia");
    map.insert("CM",	"Cameroon");
    map.insert("CA",	"Canada");
    map.insert("CV",	"Cape Verde");
    map.insert("KY",	"Cayman Islands");
    map.insert("CF",	"Central African Republic");
    map.insert("TD",	"Chad");
    map.insert("CL",	"Chile");
    map.insert("CN",	"China");
    map.insert("CX",	"Christmas Island");
    map.insert("CC",	"Cocos (Keeling) Islands");
    map.insert("CO",	"Colombia");
    map.insert("KM",	"Comoros");
    map.insert("CG",	"Congo");
    map.insert("CD",	"Congo, the Democratic Republic of the");
    map.insert("CK",	"Cook Islands");
    map.insert("CR",	"Costa Rica");
    map.insert("CI",	"Côte D'Ivoire");
    map.insert("HR",	"Croatia");
    map.insert("CU",	"Cuba");
    map.insert("CY",	"Cyprus");
    map.insert("CZ",	"Czech Republic");
    map.insert("DK",	"Denmark");
    map.insert("DJ",	"Djibouti");
    map.insert("DM",	"Dominica");
    map.insert("DO",	"Dominican Republic");
    map.insert("EC",	"Ecuador");
    map.insert("EG",	"Egypt");
    map.insert("SV",	"El Salvador");
    map.insert("GQ",	"Equatorial Guinea");
    map.insert("ER",	"Eritrea");
    map.insert("EE",	"Estonia");
    map.insert("ET",	"Ethiopia");
    map.insert("FK",	"Falkland Islands (Malvinas)");
    map.insert("FO",	"Faroe Islands");
    map.insert("FJ",	"Fiji");
    map.insert("FI",	"Finland");
    map.insert("FR",	"France");
    map.insert("GF",	"French Guiana");
    map.insert("PF",	"French Polynesia");
    map.insert("TF",	"French Southern Territories");
    map.insert("GA",	"Gabon");
    map.insert("GM",	"Gambia");
    map.insert("GE",	"Georgia");
    map.insert("DE",	"Germany");
    map.insert("GH",	"Ghana");
    map.insert("GI",	"Gibraltar");
    map.insert("GR",	"Greece");
    map.insert("GL",	"Greenland");
    map.insert("GD",	"Grenada");
    map.insert("GP",	"Guadeloupe");
    map.insert("GU",	"Guam");
    map.insert("GT",	"Guatemala");
    map.insert("GG",	"Guernsey");
    map.insert("GN",	"Guinea");
    map.insert("GW",	"Guinea-Bissau");
    map.insert("GY",	"Guyana");
    map.insert("HT",	"Haiti");
    map.insert("HM",	"Heard Island and Mcdonald Islands");
    map.insert("VA",	"Holy See (Vatican City State)");
    map.insert("HN",	"Honduras");
    map.insert("HK",	"Hong Kong");
    map.insert("HU",	"Hungary");
    map.insert("IS",	"Iceland");
    map.insert("IN",	"India");
    map.insert("ID",	"Indonesia");
    map.insert("IR",	"Iran, Islamic Republic of");
    map.insert("IQ",	"Iraq");
    map.insert("IE",	"Ireland");
    map.insert("IM",	"Isle of Man");
    map.insert("IL",	"Israel");
    map.insert("IT",	"Italy");
    map.insert("JM",	"Jamaica");
    map.insert("JP",	"Japan");
    map.insert("JE",	"Jersey");
    map.insert("JO",	"Jordan");
    map.insert("KZ",	"Kazakhstan");
    map.insert("KE",	"KENYA");
    map.insert("KI",	"Kiribati");
    map.insert("KP",	"Korea, Democratic People's Republic of");
    map.insert("KR",	"Korea, Republic of");
    map.insert("KW",	"Kuwait");
    map.insert("KG",	"Kyrgyzstan");
    map.insert("LA",	"Lao People's Democratic Republic");
    map.insert("LV",	"Latvia");
    map.insert("LB",	"Lebanon");
    map.insert("LS",	"Lesotho");
    map.insert("LR",	"Liberia");
    map.insert("LY",	"Libyan Arab Jamahiriya");
    map.insert("LI",	"Liechtenstein");
    map.insert("LT",	"Lithuania");
    map.insert("LU",	"Luxembourg");
    map.insert("MO",	"Macao");
    map.insert("MK",	"Macedonia, the Former Yugoslav Republic of");
    map.insert("MG",	"Madagascar");
    map.insert("MW",	"Malawi");
    map.insert("MY",	"Malaysia");
    map.insert("MV",	"Maldives");
    map.insert("ML",	"Mali");
    map.insert("MT",	"Malta");
    map.insert("MH",	"Marshall Islands");
    map.insert("MQ",	"Martinique");
    map.insert("MR",	"Mauritania");
    map.insert("MU",	"Mauritius");
    map.insert("YT",	"Mayotte");
    map.insert("MX",	"Mexico");
    map.insert("FM",	"Micronesia, Federated States of");
    map.insert("MD",	"Moldova, Republic of");
    map.insert("MC",	"Monaco");
    map.insert("MN",	"Mongolia");
    map.insert("ME",	"Montenegro");
    map.insert("MS",	"Montserrat");
    map.insert("MA",	"Morocco");
    map.insert("MZ",	"Mozambique");
    map.insert("MM",	"Myanmar");
    map.insert("NA",	"Namibia");
    map.insert("NR",	"Nauru");
    map.insert("NP",	"Nepal");
    map.insert("NL",	"Netherlands");
    map.insert("AN",	"Netherlands Antilles");
    map.insert("NC",	"New Caledonia");
    map.insert("NZ",	"New Zealand");
    map.insert("NI",	"Nicaragua");
    map.insert("NE",	"Niger");
    map.insert("NG",	"Nigeria");
    map.insert("NU",	"Niue");
    map.insert("NF",	"Norfolk Island");
    map.insert("MP",	"Northern Mariana Islands");
    map.insert("NO",	"Norway");
    map.insert("OM",	"Oman");
    map.insert("PK",	"Pakistan");
    map.insert("PW",	"Palau");
    map.insert("PS",	"Palestinian Territory, Occupied");
    map.insert("PA",	"Panama");
    map.insert("PG",	"Papua New Guinea");
    map.insert("PY",	"Paraguay");
    map.insert("PE",	"Peru");
    map.insert("PH",	"Philippines");
    map.insert("PN",	"Pitcairn");
    map.insert("PL",	"Poland");
    map.insert("PT",	"Portugal");
    map.insert("PR",	"Puerto Rico");
    map.insert("QA",	"Qatar");
    map.insert("RE",	"Réunion");
    map.insert("RO",	"Romania");
    map.insert("RU",	"Russian");
    map.insert("RW",	"Rwanda");
    map.insert("SH",	"Saint Helena");
    map.insert("KN",	"Saint Kitts and Nevis");
    map.insert("LC",	"Saint Lucia");
    map.insert("PM",	"Saint Pierre and Miquelon");
    map.insert("VC",	"Saint Vincent and the Grenadines");
    map.insert("WS",	"Samoa");
    map.insert("SM",	"San Marino");
    map.insert("ST",	"Sao Tome and Principe");
    map.insert("SA",	"Saudi Arabia");
    map.insert("SN",	"Senegal");
    map.insert("RS",	"Serbia");
    map.insert("SC",	"Seychelles");
    map.insert("SL",	"Sierra Leone");
    map.insert("SG",	"Singapore");
    map.insert("SK",	"Slovakia");
    map.insert("SI",	"Slovenia");
    map.insert("SB",	"Solomon Islands");
    map.insert("SO",	"Somalia");
    map.insert("ZA",	"South Africa");
    map.insert("GS",	"South Georgia and the South Sandwich Islands");
    map.insert("ES",	"Spain");
    map.insert("LK",	"Sri Lanka");
    map.insert("SD",	"Sudan");
    map.insert("SR",	"Suriname");
    map.insert("SJ",	"Svalbard and Jan Mayen");
    map.insert("SZ",	"Swaziland");
    map.insert("SE",	"Sweden");
    map.insert("CH",	"Switzerland");
    map.insert("SY",	"Syrian Arab Republic");
    map.insert("TW",	"Taiwan, Province of China");
    map.insert("TJ",	"Tajikistan");
    map.insert("TZ",	"Tanzania, United Republic of");
    map.insert("TH",	"Thailand");
    map.insert("TL",	"Timor-Leste");
    map.insert("TG",	"Togo");
    map.insert("TK",	"Tokelau");
    map.insert("TO",	"Tonga");
    map.insert("TT",	"Trinidad and Tobago");
    map.insert("TN",	"Tunisia");
    map.insert("TR",	"Turkey");
    map.insert("TM",	"Turkmenistan");
    map.insert("TC",	"Turks and Caicos Islands");
    map.insert("TV",	"Tuvalu");
    map.insert("UG",	"Uganda");
    map.insert("UA",	"Ukraine");
    map.insert("AE",	"United Arab Emirates");
    map.insert("GB",	"United Kingdom");
    map.insert("US",	"United States");
    map.insert("UM",	"United States Minor Outlying Islands");
    map.insert("UY",	"Uruguay");
    map.insert("UZ",	"Uzbekistan");
    map.insert("VU",	"Vanuatu");
    map.insert("VA",	"Vatican City State");
    map.insert("VE",	"Venezuela");
    map.insert("VN",	"Viet Nam");
    map.insert("VG",	"Virgin Islands, British");
    map.insert("VI",	"Virgin Islands, U.S.");
    map.insert("WF",	"Wallis and Futuna");
    map.insert("EH",	"Western Sahara");
    map.insert("YE",	"Yemen");
    map.insert("CD",	"Zaire");
    map.insert("ZM",	"Zambia");
    map.insert("ZW",	"Zimbabwe");
    return map;
}

const QMap<QString, QString> AbstractPeer::countriesMap = getCountries();

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
    :AbstractPeer(ID), port(0), jamtabaVersion(0), sampleRate(0)
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

//void AbstractJamRoom::addPeer(AbstractPeer* peer) {
//    if (!peers.contains(peer->getId())) {
//        peers.insert(peer->getId(); std::shared_ptr<AbstractPeer>(peer));
//        qDebug() << "Adicionou peer " << peer->getName();
//    }
//}

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
            if (dynamic_cast<RealTimePeer*>(peer)->isReachable()) {
                reachablePeers.append(dynamic_cast<RealTimePeer*>(peer));
            }
        }
    }
    else{//exclui o peer local da lista
        foreach (AbstractPeer* peer , allPeers) {
            if (dynamic_cast<RealTimePeer*>(peer)->isReachable() && peer != localPeer) {
                reachablePeers.append(dynamic_cast<RealTimePeer*>(peer));
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


//bool containBotPeer;
//QString streamLink;
//QString hostName;
//int hostPort;
//int currentBpi;
//int currentBpm;
NinjamRoom::NinjamRoom(long long ID)
    :AbstractJamRoom(ID),
      containBotPeer(false),
      streamLink(""),
      hostName(""),
      hostPort(0),
      currentBpi(0),
      currentBpm(0)
{

}

NinjamRoom::NinjamRoom(QString host, int port, int maxUsers)
    :AbstractJamRoom(1),
      containBotPeer(false),
      streamLink(""),
      hostName(host),
      hostPort(port),
      currentBpi(-1),
      currentBpm(-1)
{
    this->maxUsers = maxUsers;
}

NinjamRoom::NinjamRoom(QString host, int port, int maxUsers, int bpi, int bpm)
    :AbstractJamRoom(1),
      containBotPeer(false),
      streamLink(""),
      hostName(host),
      hostPort(port),
      currentBpi(bpi),
      currentBpm(bpm)
{
    this->maxUsers = maxUsers;
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

