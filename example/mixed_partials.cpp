//           Copyright Matthew Pulver 2018 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include <boost/math/differentiation/autodiff.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <iostream>

using namespace boost::math::differentiation;

template <typename W, typename X, typename Y, typename Z>
promote<W, X, Y, Z> f(const W& w, const X& x, const Y& y, const Z& z) {
  return exp(w * sin(x * log(y) / z) + sqrt(w * z / (x * y))) + w * w / tan(z);
}

// Derivatives calculated from symbolic differentiation by Mathematica for comparison. Script: mixed_partials.nb
constexpr std::array<const char*, 240> answers{
    "19878.40628980434922342465374997798674242532797789489",  "20731.74838274939517275508122761443159515217855975002",
    "14667.60767623939014840117674691707821648144188283774",  "1840.559936449813118734351750381849294157477519107602",
    "-9219.318005237072129605008516120710807803827373819700", "-7272.300634012811783845589472196110804386170683300081",
    "-2135.296370062283924160196772166043360841114107521292", "3095.081027251846799545897828297310835169325417217168",
    "4249.026762908615627428402369471953790564918480025345",  "2063.989061062734416582172072883742097425754355167541",
    "-885.5284114876496084068555333811894392182458751895290", "-1962.133420441743158021558423645064067562765178375508",
    "-1846.899830787084518564013512948598850243350915531775", "-160.9590127603295755195950112199107484483554942817846",
    "1091.039412341633994110997652976585409621806446647794",  "452.4395574345229946707651998323417632800605985181691",
    "666.4013922727704990031159406121675703174518834914461",  "-415.6464114333629107803309520898363153301435468382605",
    "-625.1464179039986361267627631122900331946746137220517", "369.9491669772617110087494756677334192842413470837587",
    "-24330.89613849389343130420303653062335840497802221681", "-18810.41605175626752065686192937776868736029049989926",
    "-4890.406122702359099863022925593448420259414896197252", "8833.005054768976417065486877649473665597894570245307",
    "8484.350739681613747819854384228795938450532463850094",  "3097.204151240398893507362023543393154680147349049848",
    "-3255.045136783440612110181337652522080890693968833148", "-4342.778553332193097878812792875447018366988006584840",
    "-2407.987237906523486012534085031032446996713414362131", "861.1173916470300084261504495377425043024739914571554",
    "2436.743725763308619092960749816106318692933687303014",  "-19.24649610733827783846392798978023489104363382129689",
    "187.7855148870511714395275130898958731897480766620821",  "-1259.466063335212195169531010871023748854744563232277",
    "-709.6860523972158261343923419671629587637051060458295", "1423.000558608604536932163648918899935569543711292466",
    "484.9208133389233959103861107714757012185008046446372",  "763.9746885074453180462508029718247316712990115789154",
    "-327.4162918228055568224139277603073169658358026440432", "-1122.337707248494521123614369562896901904418640152220",
    "23973.06007192346989337502250398494874845408708506720",  "8840.543151778796869949670401421984604862699128880003",
    "-9082.571033221549378277312292526023838132689941236879", "-12270.27378289258717737657881957466807305650429436397",
    "-4320.434071420599854743576892819691675331049612545664", "3281.351967707280898543984556670710235259118405463698",
    "5880.336263083418767219493592767818708317492833223933",  "-1288.482785219706549809211085113790275109642879331959",
    "-803.9713537626580526627976840414468844364935388365037", "-2986.387245331698390346145949708414455858834967096376",
    "-586.7316859822658306283656047992829723003491823675739", "3929.073189280739356198769778905960586080418779863615",
    "1453.728280983826630077825553258703050898056317382483",  "1037.878071685953829685046234106860743366780050925514",
    "-1482.745805277401336553926171580259185140208053329753", "-1877.134792933828810602377451370316364621357891989679",
    "-931.7138710369298207131581126980851620513905805624544", "254.6565590420322632851077818917210811815919344882311",
    "1391.248064745611663849820246430123214796614030838600",  "-431.4820563154137955051720207563800896297257103310465",
    "16975.34005365179555009050533000516107937041784876054",  "19662.60356303341709846238790020024593550984564081068",
    "15765.85130704020004301064240357947656083104783442825",  "3972.155036195937013764185795634749937308876197976202",
    "-8681.748539789720512499473840242996096730194203989543", "-7703.183042460387656743498394861780784700076575106134",
    "-3049.708696569518774040135942468704911634779352213044", "2971.469685992270876159892302788930292108129670398058",
    "4370.196499857550025657084783894747734031876677385611",  "2524.632473357435670756946837415389227139966527203701",
    "-656.6080000236679071742450437463693211275208125750923", "-2423.452917325258132591368397957959217829861665178601",
    "-2074.987664204263204162199830716851483704870169031179", "-381.2253794988132984501358802316138392247470857452486",
    "1219.507245791997351017860252538035146744682380716428",  "805.3802239840836877339667281819652171888443003165988",
    "838.4004190058912380470543219448821914235443115661655",  "-390.6125197108983831575656956558201636111305409512701",
    "-828.2085489298235758253219930356006757081473789845849", "293.8999854454994790079171865082094494146506490533363",
    "-22965.85985843951977785883587223006628792405076928067", "-20026.69101529929621743747554537576887048069629325374",
    "-7316.092745063355996548975300169565482331369744607021", "8632.466133972614659252310985982644793465043032940318",
    "8987.046882870452266200748127338744248816756004290490",  "4199.925399536137541108783465785304128965582292174062",
    "-2958.429850896062893179851696175634522187021390095560", "-5665.563891218624062243686482808197054863235184904433",
    "-2945.404552250341615883104643651287431663294281737652", "555.6566272478262524735403145861484390537770707372992",
    "2936.796403550079139218970638242013974322758744804216",  "651.5191650747110008135060635556227666232180743487328",
    "444.7629427486155148584918602702161457622049333694568",  "-1390.989671799095801316658971275073184600067187023729",
    "-1142.861468946763860859271224968631944511098747155437", "1541.978723117340843491920690654997335632919116206279",
    "455.7146063293814470171599782651235242129856311098151",  "998.7943503940357037260061331795191352937661538946216",
    "-204.8485581981121295383497187536442450324011940647949", "-1560.354115460478786113711476250386112014306509906244",
    "25278.29450605247223516529112562423587288781657290275",  "11873.22337179046469888005044109378787446671408425048",
    "-8242.187303368878103323785658604027555126374435611949", "-15939.98056417465751946455567789306872745912255628512",
    "-5648.833539698031486810309720694416837861242341227280", "2751.513926122717118525029734574022921057261239749143",
    "7349.432002479077129245930487320138527887196396579062",  "194.9972545980371127390142753318206783334452047502143",
    "-402.8156857682688265622049800462325595907987257153782", "-3518.871908683063371167722463713374376552181380727802",
    "-1494.304793474682619087166400375396721307777439607909", "4640.927509426080087451995953783429589632369803588940",
    "1585.757705203227141964561144798400703219894640413562",  "1565.169992404407137888592924342582799362959736185298",
    "-1513.259809733540018859089666188672238777297615451800", "-2974.437872674680092826212901753475972242208819679978",
    "-1203.236292653823441598437153564865951527142648802876", "72.52425949879153384040698301599842998884036742649047",
    "1871.625274253419949517250818647194858608124560073483",  "-2.489984337379681666361341362948045621969765070197429",
    "14462.74423518633102580192225823524237502860825596609",  "18367.74740916432711689913219912502810575714860430297",
    "16565.76324499673961400925630526921000337443450249297",  "6054.315252651102952034254100792777051580892954459740",
    "-8084.981271982030146065497115893934803061545998433631", "-7988.314359128201297240919364015959817416101519999194",
    "-3989.319346941492698525859335371231602272119870228687", "2616.721186534649016680934493970036169897788778926434",
    "4420.859270970486562095630193355634655337290952862363",  "2973.033519764547909146474824627687039969488363657908",
    "-324.1453016982713707989332262410969595194473127209825", "-2843.242039958969221918101261762794653424879358390111",
    "-2281.461806143289517702658392470195144560150025832652", "-642.9353229582055924928927665183236308235598082837497",
    "1299.287274176955358490409470855361289523321919337117",  "1238.597083372069762230817383681570828675426312803376",
    "1021.334042770848165110529668635291528449691525937968",  "-329.0529345069271079573348500899329811170455711610811",
    "-1046.254301544052075124857362060924818517694048905299", "134.7343039554480655186788228552325941588620079791654",
    "-21431.41643507661192392650726158493697457993678274754", "-20856.88281479015784660571401663659059349708627445067",
    "-9829.261970591930907585958999196966814861251125275804", "7806.858647077811827981774785577363365546600234846335",
    "9319.700085649568180114405924685286453652118439999060",  "5319.898768025758256383579171601100187435481641933401",
    "-2387.954826466841736373447020403170264502066930376059", "-6958.298525165359760665355886221309296550746152109847",
    "-3468.539106391972560670887295398968213297736424267559", "130.4167253342709401698825285623058661085645012029873",
    "3371.139930235175987370940343096776588915600470241960",  "1569.232678004908105313880673484968847566948896728142",
    "750.0912101179065245750415609380442359608197763310413",  "-1462.257209626597452197736652121394535208578921869658",
    "-1661.577809630240615684355192771059515041884351493459", "1509.628528603869133250456671040505284128185908768108",
    "383.8950902580816259502239917715884779698864996879279",  "1248.051096343638013308778159911906703363730187986273",
    "17.18569564265260274901760034571610990094333217519021",  "-2038.024598002604853054532645991188063394308018947374",
    "26118.98132017823514803387529120810044029492871875474",  "14943.61943482227903328457116850255971625430735856355",
    "-6650.686262276131072415580833374348889422387492668440", "-19519.81529547404067945704333355155941895199228108631",
    "-6983.190236500848647457042860591724089812405118922223", "1899.297502873688983038424995203515277346497811783168",
    "8715.003652642963488202943622358986745434720576722170",  "2368.150690681864301926962120618658083737878227231428",
    "136.8920793093482831910443246272238406481527839521448",  "-3954.732706163417141961077488373290331419627965482785",
    "-2673.556440231186786375595871506657802723673830409989", "5078.483935249043594670125721926702845818403229980691",
    "1643.459143721204817182772630730123271413273760820347",  "2182.216979506380293664703833586468523416961563720645",
    "-1345.838830963620501537777318021157952722412472356094", "-4309.285350629108413525304135326225818270616857298235",
    "-1488.050869922417817689426519211523527088509094291312", "-228.0584943070343720919835603886532454450555855354340",
    "2373.398940425709177876367020236623713151456855728138",  "773.8481328103928058186643458500631723389600248582833",
    "12294.40387737855548614823173849184004455244840062464",  "16977.34966571858301862913845572077593071467784570724",
    "17057.17475622503175013658695220988017704387344177727",  "8121.189758511830935868344768490586007624092305459885",
    "-7458.443541406284389918808653948439156033975014107187", "-8134.131160882738058651976911725365291142418949378248",
    "-4912.881158613784419581465435995807691111897279859302", "2030.653136098933717888434825960516061206391833398177",
    "4407.490527709412730881592594976776779312299897714205",  "3392.434568825892752350943548729559313328141534290860",
    "104.0372355841506198680609232049783930050635078746762",  "-3180.817620484463214391157460812371170723810181051096",
    "-2460.523987075069437321629265332968914260047631079537", "-938.2209314069133432825590545267820890922150850657831",
    "1315.246905571876456706320919211807375254975062430487",  "1735.862392405992188189147617586418269768276241147998",
    "1209.759657223166954850207025399731503326968841680649",  "-227.3320054566642297128407910803774238020746116287390",
    "-1266.126209991929259396966729664100401813091860201682", "-123.0794572338149156803989321165094334755661021559442",
    "-19806.90794333834685506732819834090525250045748665845", "-21314.81635440575229337844631555492486744407550254908",
    "-12317.58384430130805020250005527399703840208659666608", "6349.418659888281474363154227419204673663621492760982",
    "9489.819687696527735093973063679592839666155440941289",  "6409.538948456309994399374417972222747225748405617373",
    "-1550.281799013125267606263057621300789555474258987989", "-8109.711199785217512061886243157800006692908759687186",
    "-3957.840330296874877742767473517819198882831790006004", "-404.0796555836667858753163727999380679499192203780272",
    "3693.614351301181980145006883746936633676934626580499",  "2716.146658322790064799415509615557123789406209068981",
    "1094.591086641398900496318896947912437274250932576747",  "-1456.269645549946420883827817869876763706452982413420",
    "-2244.380608735636962338392373719455877272151458411079", "1268.593891556261871090883000459505759446497182073132",
    "265.2206730327749346649809229271069944357537135668622",  "1496.091578778639488439197917198148587432113387871024",
    "354.6137351047722781932932090799444060236757625488818",  "-2508.477110048684129181005769771219369377836598443263",
    "26517.86140875157324686379805134248778305979287686214",  "17922.98387741915144079932445041215068937644694653527",
    "-4328.259142127668040873054918170572859673703425721293", "-22704.70245940080949074466622805971940616027152354999",
    "-8268.613747173738971390434576274225941735552759965376", "740.4056074392611464740778308961471299437619012164253",
    "9848.900182836035080973766381422758538530595451048714",  "5213.598341476210337710365441072904970861063876340963",
    "801.2462923723508233330997243930793458484750729415321",  "-4241.870133920767845856621968904769727964770527614244",
    "-4092.241355868550570635569815488217469506874233892269", "5074.435909206083943809967780457349942315503368249477",
    "1607.765329254820915989772546102530187884674235100928",  "2861.155651116567526208762405651011317435252198548496",
    "-918.9310546317296090214320737728927500362088478158839", "-5803.211323646092019259074499814222806376618363553826",
    "-1767.541897994477314401145980308432268207111761980100", "-663.0646207520075726320417301262932382663072876188661",
    "2837.903194613938414496183429129769829434890424213252",  "1976.319600747797717779881875290418720908121189218755"};

int main() {
  using type = boost::multiprecision::cpp_bin_float_50;
  constexpr int Nw = 3;  // Max order of derivative to calculate for w
  constexpr int Nx = 2;  // Max order of derivative to calculate for x
  constexpr int Ny = 4;  // Max order of derivative to calculate for y
  constexpr int Nz = 3;  // Max order of derivative to calculate for z
  const auto w = make_fvar<type, Nw>(11);
  const auto x = make_fvar<type, 0, Nx>(12);
  const auto y = make_fvar<type, 0, 0, Ny>(13);
  const auto z = make_fvar<type, 0, 0, 0, Nz>(14);
  const auto v = f(w, x, y, z);  // auto = autodiff_fvar<type,Nw,Nx,Ny,Nz>
  int ia = 0;
  double max_relative_error = 0;
  for (std::size_t iw = 0; iw <= Nw; ++iw)
    for (std::size_t ix = 0; ix <= Nx; ++ix)
      for (std::size_t iy = 0; iy <= Ny; ++iy)
        for (std::size_t iz = 0; iz <= Nz; ++iz) {
          const type value = v.derivative(iw, ix, iy, iz);
          const type answer = boost::lexical_cast<type>(answers[static_cast<std::size_t>(ia++)]);
          const double error = static_cast<double>(fabs(value / answer - 1));
          max_relative_error = (std::max)(error, max_relative_error);
        }
  std::cout << "max_relative_error = " << std::setprecision(3) << max_relative_error << " out of " << ia
            << " calculated values." << std::endl;
  return 0;
}
/*
Output:
max_relative_error = 6.82e-13 out of 240 calculated values. (for type=double)
max_relative_error = 3.3e-47 out of 240 calculated values. (for type=cpp_bin_float_50)
**/
