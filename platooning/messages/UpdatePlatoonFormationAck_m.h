//
// Generated file, do not edit! Created by nedtool 5.4 from veins/modules/application/platooning/messages/UpdatePlatoonFormationAck.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __UPDATEPLATOONFORMATIONACK_M_H
#define __UPDATEPLATOONFORMATIONACK_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "ManeuverMessage_m.h"
// }}

/**
 * Class generated from <tt>veins/modules/application/platooning/messages/UpdatePlatoonFormationAck.msg:27</tt> by nedtool.
 * <pre>
 * // Message from all vehicles in the Platoon to the leader to acknoledge the updated formation.
 * // Is similar to a PlatoonBeacon.
 * packet UpdatePlatoonFormationAck extends ManeuverMessage
 * {
 *     double platoonSpeed;
 *     int platoonLane;
 *     int platoonFormation[];
 * }
 * </pre>
 */
class UpdatePlatoonFormationAck : public ::ManeuverMessage
{
  protected:
    double platoonSpeed;
    int platoonLane;
    int *platoonFormation; // array ptr
    unsigned int platoonFormation_arraysize;

  private:
    void copy(const UpdatePlatoonFormationAck& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const UpdatePlatoonFormationAck&);

  public:
    UpdatePlatoonFormationAck(const char *name=nullptr, short kind=0);
    UpdatePlatoonFormationAck(const UpdatePlatoonFormationAck& other);
    virtual ~UpdatePlatoonFormationAck();
    UpdatePlatoonFormationAck& operator=(const UpdatePlatoonFormationAck& other);
    virtual UpdatePlatoonFormationAck *dup() const override {return new UpdatePlatoonFormationAck(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual double getPlatoonSpeed() const;
    virtual void setPlatoonSpeed(double platoonSpeed);
    virtual int getPlatoonLane() const;
    virtual void setPlatoonLane(int platoonLane);
    virtual void setPlatoonFormationArraySize(unsigned int size);
    virtual unsigned int getPlatoonFormationArraySize() const;
    virtual int getPlatoonFormation(unsigned int k) const;
    virtual void setPlatoonFormation(unsigned int k, int platoonFormation);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const UpdatePlatoonFormationAck& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, UpdatePlatoonFormationAck& obj) {obj.parsimUnpack(b);}


#endif // ifndef __UPDATEPLATOONFORMATIONACK_M_H

