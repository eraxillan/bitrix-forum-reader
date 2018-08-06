#ifndef WEBSITEINTERFACE_QT_H
#define WEBSITEINTERFACE_QT_H

#include <QtCore/QDate>

#include <website_backend/websiteinterface_fwd.h>

class PostQtWrapper
{
    Q_GADGET

    QString m_authorQml;
    QString m_authorSignature;
    QDateTime m_date;
    QString m_contentQml;
    QString m_lastEdit;
    int m_likeCount;

    Q_PROPERTY(QString authorQml READ authorQml)
    Q_PROPERTY(QString authorSignature READ authorSignature)
    Q_PROPERTY(QDateTime date READ date)
    Q_PROPERTY(QString contentQml READ contentQml)
    Q_PROPERTY(QString lastEdit READ lastEdit)
    Q_PROPERTY(int likeCount READ likeCount)

public:
    PostQtWrapper();
    PostQtWrapper(bfr::PostPtr post);

    QString authorQml() const;
    QString authorSignature() const;
    QDateTime date() const;
    QString contentQml() const;
    QString lastEdit() const;
    int likeCount() const;
};

class UserQtWrapper
{
    Q_GADGET

    QString m_name;
    QString m_avatarUrl;
    QDate m_registrationDate;
    QString m_city;
    int m_postCount;
    int m_threadPostCount;
    qreal m_reputation;
    qreal m_threadReputation;
    qreal m_ratio;
    qreal m_threadRatio;

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString avatarUrl READ avatarUrl)
    Q_PROPERTY(QDateTime registrationDate READ registrationDate)
    Q_PROPERTY(QString city READ city)
    Q_PROPERTY(int postCount READ postCount)
    Q_PROPERTY(int threadPostCount READ threadPostCount)
    Q_PROPERTY(qreal reputation READ reputation)
    Q_PROPERTY(qreal threadReputation READ threadReputation)
    Q_PROPERTY(qreal ratio READ ratio)
    Q_PROPERTY(qreal threadRatio READ threadRatio)

public:
    UserQtWrapper();
    UserQtWrapper(bfr::UserPtr user);

    QString name() const;
    QString avatarUrl() const;
    QDate registrationDate() const;
    QString city() const;
    int postCount() const;
    int threadPostCount() const;
    qreal reputation() const;
    qreal threadReputation() const;
    qreal ratio() const;
    qreal threadRatio() const;
};

Q_DECLARE_METATYPE(PostQtWrapper)
Q_DECLARE_METATYPE(UserQtWrapper)

#endif // WEBSITEINTERFACE_QT_H
