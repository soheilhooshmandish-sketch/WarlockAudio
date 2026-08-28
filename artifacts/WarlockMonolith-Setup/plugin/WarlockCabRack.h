#pragma once
namespace warlock {
enum class CabModel : int { Off = 0, Hemp412, Ceramic412, Vintage412, Open212, Bin15, DiIron, Count };
enum class CabMic : int { Axis = 0, Edge, Room, Count };
inline const char* cabModelName (CabModel m) {
  switch (m) {
    case CabModel::Hemp412: return "HEMP 4x12"; case CabModel::Ceramic412: return "CERAMIC 4x12";
    case CabModel::Vintage412: return "VINTAGE 4x12"; case CabModel::Open212: return "OPEN 2x12";
    case CabModel::Bin15: return "BIN 1x15"; case CabModel::DiIron: return "DI IRON"; default: return "OFF";
  }
}
inline const char* cabMicName (CabMic m) {
  switch (m) { case CabMic::Edge: return "EDGE"; case CabMic::Room: return "ROOM"; default: return "AXIS"; }
}
inline const char* cabIrFile (CabModel cab, CabMic mic) {
  switch (cab) {
    case CabModel::Hemp412: return mic == CabMic::Edge ? "IR_Hemp412_Edge.wav" : (mic == CabMic::Room ? "IR_Hemp412_Room.wav" : "IR_Hemp412_Axis.wav");
    case CabModel::Ceramic412: return mic == CabMic::Edge ? "IR_Ceramic412_Edge.wav" : (mic == CabMic::Room ? "IR_Ceramic412_Room.wav" : "IR_Ceramic412_Axis.wav");
    case CabModel::Vintage412: return mic == CabMic::Edge ? "IR_Vintage412_Edge.wav" : (mic == CabMic::Room ? "IR_Vintage412_Room.wav" : "IR_Vintage412_Axis.wav");
    case CabModel::Open212: return mic == CabMic::Edge ? "IR_Open212_Edge.wav" : (mic == CabMic::Room ? "IR_Open212_Room.wav" : "IR_Open212_Axis.wav");
    case CabModel::Bin15: return mic == CabMic::Edge ? "IR_Bin15_Edge.wav" : (mic == CabMic::Room ? "IR_Bin15_Room.wav" : "IR_Bin15_Axis.wav");
    case CabModel::DiIron: return "IR_DiIron.wav"; default: return nullptr;
  }
}
struct CabRackKnobs {
  CabModel model = CabModel::Hemp412; CabMic mic = CabMic::Edge;
  float mix = 0.30f; float lowCut = 55.f; float highCut = 7000.f; bool loFi = false; bool stereo = true;
};
}
