#include "JamRoomViewPanel.h"
#include "ui_jamroomviewpanel.h"
#include "../loginserver/JamRoom.h"

#include <QPainter>
#include <QDebug>
#include <QLocale>

static QMap<QString, QString> countriesMap = {
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


JamRoomViewPanel::JamRoomViewPanel(QWidget *parent)
    :QWidget(parent)
{
    ui->labelRoomStatus->setText("");
}

JamRoomViewPanel::JamRoomViewPanel(Login::AbstractJamRoom* jamRoom, QWidget* parent)
    :QWidget(parent),
      ui(new Ui::RoomViewPanel),
      currentRoom(jamRoom)
{
    ui->setupUi(this);
    initialize();
}

void JamRoomViewPanel::addPeak(float peak){
    ui->wavePeakPanel->addPeak(peak);
}

void JamRoomViewPanel::paintEvent( QPaintEvent */*e*/ ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void JamRoomViewPanel::initialize(){
    ui->labelName->setText(currentRoom->getName());
    ui->labelRoomType->setText( currentRoom->getRoomType() == Login::AbstractJamRoom::Type::NINJAM ? "Ninjam" : "Realtime" );
    if(currentRoom->isEmpty()){
        ui->labelRoomStatus->setText( "Empty room!"  );
    }
    else if(currentRoom->isFull()){
        ui->labelRoomStatus->setText( "Crowded room!");
    }
    else{
        ui->labelRoomStatus->setText("");
    }

    foreach (Login::AbstractPeer* user, currentRoom->getPeers()) {
        if(!user->isBot()){
            QLabel* label = new QLabel(ui->usersPanel);
            label->setTextFormat(Qt::RichText);
            QString countryCode = user->getCountryCode().toLower();
            QString countryName = countriesMap[user->getCountryCode()];
            QString userString = user->getName() + " <i>(" + countryName + ")</i>";
            label->setText("<img src=:/flags/flags/" + countryCode +".png> " + userString);
            ui->usersPanel->layout()->addWidget(label);
        }
    }

    ui->buttonListen->setEnabled(currentRoom->hasStreamLink());
}

JamRoomViewPanel::~JamRoomViewPanel()
{
    delete ui;
}

void JamRoomViewPanel::clearPeaks(){
    ui->wavePeakPanel->clearPeaks();
}

void JamRoomViewPanel::on_buttonListen_clicked()
{
    if(currentRoom->hasStreamLink()){
        if(ui->buttonListen->isChecked()){
            emit startingListeningTheRoom(currentRoom);
        }
        else{
            emit finishingListeningTheRoom(currentRoom);
        }
    }
}

void JamRoomViewPanel::on_buttonEnter_clicked()
{
    if(currentRoom){
        emit enteringInTheRoom(currentRoom);
    }
}
