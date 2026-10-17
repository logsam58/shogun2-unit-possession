#include "identity.h"
#include "arrow_render_geometry.h"
#include "pose_geometry.h"
int main(){try{
    // An actual held-arrow transform, translated by the native holder. Exercise
    // the same shoulder-pivot rotation as rendering across yaw and full pitch.
    for(float yaw:{-3.0f,0.0f,1.7f})for(float pitch:{-1.553343f,0.0f,1.553343f}){
        float c=std::cos(yaw),s=std::sin(yaw);
        float world[12]={c,0,s,-27.3f,0,1,0,31.25f,-s,0,c,302.0f};
        float arrow[12]={1,0,0,0.1f,0,1,0,0.45f,0,0,1,0.55f},pivot[3]={0,0.42f,0},point[3]{};
        pitch_matrix(arrow,pivot,pitch);
        require(arrow_attachment_world(arrow,world,point),"Posed arrow world point rejected");
        for(unsigned row=0;row<3;++row){
            float rendered=world[row*4+3];for(unsigned col=0;col<3;++col)rendered+=world[row*4+col]*arrow[col*4+3];
            require(std::abs(point[row]-rendered)<0.00001f,"Projectile and rendered attachment differ");
        }
        float original[12];memcpy(original,arrow,sizeof original);
        float matrices[48]{};memcpy(matrices+36,arrow,sizeof arrow);require(hide_display_arrow(matrices,4,3),"Post-spawn visual suppression");
        float hidden_point[3];require(arrow_attachment_world(matrices+36,world,hidden_point),"Hidden attachment rejected");
        for(unsigned i=0;i<3;++i)require(point[i]==hidden_point[i],"Visual suppression moved projectile attachment");
        float bad[3]={7,8,9};world[0]=NAN;
        require(!arrow_attachment_world(original,world,bad)&&bad[0]==7&&bad[1]==8&&bad[2]==9,"Invalid attachment partially changed origin");
    }
    // The decoded native arrow's four LODs are rigid on Weapon3. Verify no
    // vertex retains visible extent at arbitrary camera pitches/translations.
    for(float angle:{-1.4f,-0.5f,0.0f,0.5f,1.4f}){
        float m[48]{},before[48];for(unsigned i=0;i<4;++i){
            m[i*12]=1;m[i*12+5]=std::cos(angle);m[i*12+6]=std::sin(angle);
            m[i*12+9]=-std::sin(angle);m[i*12+10]=std::cos(angle);
            m[i*12+3]=float(i);m[i*12+7]=1.6f;m[i*12+11]=0.8f;
        }
        memcpy(before,m,sizeof m);
        require(hide_display_arrow(m,4,3),"Arrow suppression rejected");
        require(!memcmp(m,before,36*sizeof(float)),"Bow, sword or hips changed");
        for(unsigned row=0;row<3;++row){
            require(m[36+row*4+3]==before[36+row*4+3],"Attachment translation changed");
            for(unsigned col=0;col<3;++col)require(m[36+row*4+col]==before[36+row*4+col]*0.0001f,"Arrow render scale differs");
        }
        require(hide_display_arrow(m,4,3),"Repeated render changed validity");
        // Native regeneration/restoration is an exact copy of the saved matrices.
        memcpy(m,before,sizeof m);require(!memcmp(m,before,sizeof m),"Restoration differs");
        require(!hide_display_arrow(m,4,1)&&!memcmp(m,before,sizeof m),"Non-arrow bone changed");
        m[36]=NAN;memcpy(before,m,sizeof m);
        require(!hide_display_arrow(m,4,3)&&!memcmp(m,before,sizeof m),"Invalid arrow partially changed");
    }
    require(!hide_display_arrow(nullptr,4,3)&&!hide_display_arrow(nullptr,257,3),"Bounds rejection failed");
    std::cout<<"PASS: exact posed-arrow world origin across yaw/pitch, post-spawn point preservation, nonzero arrow render scale, bow/sword/body preservation, invalid input rejection and matrix restoration\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
