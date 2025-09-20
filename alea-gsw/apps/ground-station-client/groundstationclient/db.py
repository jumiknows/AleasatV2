from sqlalchemy import Boolean, Column, Float, String, create_engine
from sqlalchemy.dialects.sqlite.json import JSON
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import scoped_session, sessionmaker

Base = declarative_base()


class ContactProfile(Base):
    __tablename__ = 'contact_profile'

    id = Column(String, primary_key=True)
    downlink_destination_ips = Column(JSON, nullable=False)
    uplink_origin_ip = Column(String, nullable=False)
    center_frequency_mhz = Column(Float, nullable=False)
    bandwidth_mhz = Column(Float, nullable=False)
    save_iq_files = Column(Boolean, nullable=False)

    def to_dict(self):
        return {
            'id': self.id,
            'downlinkDestinationIPs': self.downlink_destination_ips,
            'uplinkOriginIP': self.uplink_origin_ip,
            'centerFrequencyMHz': self.center_frequency_mhz,
            'bandwidthMHz': self.bandwidth_mhz,
            'saveIQFiles': self.save_iq_files
        }


engine = create_engine('sqlite:///database.sqlite')
Base.metadata.create_all(engine)

Session = scoped_session(sessionmaker(bind=engine))
