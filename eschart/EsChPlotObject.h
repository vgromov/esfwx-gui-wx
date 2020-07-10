#ifndef _es_ch_plot_object_h_
#define _es_ch_plot_object_h_

// Forward declaration
class EsChPlotObjectHitInfo;
class EsChPlot;

/// @brief  Plot object base events notification propagation. 
///         Implemented using publisher-subscriber pattern
///         
class ESCHART_CLASS EsChPlotObject
{
protected:
	// pending flags
	enum {
		PENDING_RECALC = 1,
		PENDING_REDRAW = 2,
	};

public:
	typedef std::shared_ptr<EsChPlotObject> Ptr;

	// scope locker class
	class ESCHART_CLASS Lock
	{
	public:
		Lock(const EsChPlotObject::Ptr& obj);
		~Lock();

	private:
		Lock(const Lock&);
		const Lock& operator=(const Lock&);

	protected:
		EsChPlotObject::Ptr m_obj;
	};
	friend class Lock;

protected:
	EsChPlotObject(EsChPlotObject* obj = 0);

public:
	virtual ~EsChPlotObject() {}

public:
	// notifier interface
	void notifyRecalc() const
	{
		notifyRecalc(*this);
	}
	void notifyRedraw() const
	{
		notifyRedraw(*this);
	}

	// subscriber-publisher linkage services
	void subscriberAdd(EsChPlotObject* obj);
	void subscriberRemove(EsChPlotObject* obj);

	// hit testing interface. declared public, though
	// should not be called directly unless you know what you're
	// doing
	virtual bool hitInfoGet(const EsChPlot& plot, double wx, double wy, 
		double toleranceX, double toleranceY,
		EsChPlotObjectHitInfo& info) const;

protected:
	// internal notification services
	void notifyRecalc(const EsChPlotObject& obj) const;
	void notifyRedraw(const EsChPlotObject& obj) const;
	// notificatoin handlers
	virtual void onRecalc(const EsChPlotObject& sender);
	virtual void onRedraw(const EsChPlotObject& sender) const;
	// lock handling services
	void lock();
	void unlock();
	bool isLocked() const
	{
		return m_lock > 0;
	}
	virtual void onUnlock();

private:
	// prohibited functionality
	EsChPlotObject(const EsChPlotObject&);
	const EsChPlotObject& operator=(const EsChPlotObject&);

protected:
	// lock state
	size_t m_lock;
	mutable size_t m_pendingFlags;
	// plots, this object is linked to. size_t value contains object link count,
	// cause it may be linked to the different views of the same plot
	typedef std::map<EsChPlotObject*, size_t> Subscribers;
	Subscribers m_subscribers;
};

// plot object hit info helper class
//
class ESCHART_CLASS EsChPlotObjectHitInfo
{
public:
	// hit info codes
	enum Code {
		NONE,
		BODY,
		POINT,
		BORDER,
	};

public:
	EsChPlotObjectHitInfo(Code code = NONE, const EsChPlotObject* obj = 0, const EsVariant& optional = EsVariant::null());
	EsChPlotObjectHitInfo(const EsChPlotObjectHitInfo& other);
	const EsChPlotObjectHitInfo& operator=(const EsChPlotObjectHitInfo& other);

	// members r-o access
	bool isHit() const
	{
		return m_code != NONE;
	}
	EsChPlotObjectHitInfo::Code codeGet() const
	{
		return m_code;
	}
	const EsChPlotObject* objectGet() const
	{
		return m_obj;
	}
	const EsVariant& optionalGet() const
	{
		return m_optional;
	}

	// static services
	//
	static EsString codeStringGet(size_t code);

protected:
	void copy(const EsChPlotObjectHitInfo& other);

protected:
	Code m_code;
	const EsChPlotObject* m_obj;
	// additional optional information
	EsVariant m_optional;
};

// plot decoration object base
// this is visual plot object, which may be added to the view in z-order above the series
// and is capable of rendering itself to the plot
//
class ESCHART_CLASS EsChPlotDecorationObject : public EsChPlotObject
{
public:
	typedef std::shared_ptr<EsChPlotDecorationObject> Ptr;
	typedef std::vector<Ptr> Array;

protected:
	virtual void render(const EsChPlot& plot) const = 0;		

	friend class EsChPlot;
};

#endif // _es_ch_plot_objecct_h_
