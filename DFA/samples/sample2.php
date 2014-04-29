<?php

namespace Mktp\VendorBundle\Entity;

use Doctrine\ORM\Mapping as ORM;

use Symfony\Component\Validator\Constraints as Assert;
use Symfony\Bridge\Doctrine\Validator\Constraints\UniqueEntity;

/**
 * VendorData
 *
 * @ORM\Table(name="vendor_data")
 * @ORM\Entity
 * @UniqueEntity("vendor")
 * @UniqueEntity("vendorName")
 * @UniqueEntity("vendorEmail")
 */
class VendorData
{
    /**
     * @var integer
     *
     * @ORM\Column(name="id", type="integer", nullable=false)
     * @ORM\Id
     * @ORM\GeneratedValue(strategy="AUTO")
     * @Assert\Range(
     *     min = "1",
     *     max = "2147483647",
     *     minMessage = "VendorData id is required and is a number between {{ min }} and {{ max }}.",
     *     maxMessage = "VendorData id is required and is a number between {{ min }} and {{ max }}.",
     *     invalidMessage = "VendorData id is required and is a number between {{ min }} and {{ max }}. "
     * )
     */
    private $id;

    /**
     * @var integer
     *
     * @ORM\OneToOne(targetEntity="Vendor", inversedBy="vendorData", fetch="EXTRA_LAZY")
     * @ORM\JoinColumn(name="vendor_id", referencedColumnName="id")
     * @Assert\Range(
     *     min = "1",
     *     max = "2147483647",
     *     minMessage = "VendorData vendor id is required and is a number between {{ min }} and {{ max }}.",
     *     maxMessage = "VendorData vendor id is required and is a number between {{ min }} and {{ max }}.",
     *     invalidMessage = "VendorData vendor id is required and is a number between {{ min }} and {{ max }}."
     * )
     * @Assert\NotNull()
     */
    private $vendor;

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_name", type="string", length=100, nullable=false, unique=true)
     * @Assert\Length(
     *     min = "1",
     *     max = "100",
     *     minMessage = "VendorData name is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData name is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotBlank(
     *     message = "VendorData name is required."
     * )
     */
    private $vendorName;

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_order_number", type="string", length=100, nullable=false)
     * @Assert\Length(
     *     min = "1",
     *     max = "100",
     *     minMessage = "VendorData order number is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData order number is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotBlank(
     *     message = "VendorData order number is required."
     * )
     */
    private $vendorOrderNumber;

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_cui", type="string", length=100, nullable=false)
     * @Assert\Length(
     *     min = "1",
     *     max = "100",
     *     minMessage = "VendorData cui is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData cui is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotBlank(
     *     message = "VendorData cui is required."
     * )
     */
    private $vendorCui;

    /**
     * @var float
     *
     * TODO decimal regex
     * @ORM\Column(name="vendor_capital", type="decimal", nullable=false)
     *
     * #@Assert\Regex(
     * #    pattern="/([0-9]{1,15})([\.][0-9]{1,2})?/",
     * #    match=false,
     * #    message="VendorData capital '{{ value }}' is not a valid decimal number(15,2)."
     * #)
     * @Assert\NotNull()
     */
    private $vendorCapital = 0.0;

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_iban", type="string", length=100, nullable=false)
     * @Assert\Length(
     *     min = "1",
     *     max = "100",
     *     minMessage = "VendorData iban is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData iban is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotBlank(
     *     message = "VendorData iban is required."
     * )
     */
    private $vendorIban;

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_bank", type="string", length=100, nullable=false)
     * @Assert\Length(
     *     min = "1",
     *     max = "100",
     *     minMessage = "VendorData bank is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData bank is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotBlank(
     *     message = "VendorData bank is required."
     * )
     */
    private $vendorBank;

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_address", type="string", length=200, nullable=false)
     * @Assert\Length(
     *     min = "1",
     *     max = "200",
     *     minMessage = "VendorData address is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData address is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotBlank(
     *     message = "VendorData address is required."
     * )
     */
    private $vendorAddress;

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_phone", type="string", length=30, nullable=false)
     * @Assert\Length(
     *     min = "1",
     *     max = "30",
     *     minMessage = "VendorData phone is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData phone is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotNull()
     */
    private $vendorPhone = '';

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_fax", type="string", length=30, nullable=false)
     * @Assert\Length(
     *     min = "1",
     *     max = "30",
     *     minMessage = "VendorData fax is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData fax is required and must have maximum {{ max }} characters."
     * )
     * @Assert\NotNull()
     */
    private $vendorFax = '';

    /**
     * @var string
     *
     * @ORM\Column(name="vendor_email", type="string", length=100, nullable=false, unique=true)
     * @Assert\Length(
     *     min = "1",
     *     max = "100",
     *     minMessage = "VendorData e-mail is required and must have at least {{ min }} characters.",
     *     maxMessage = "VendorData e-mail is required and must have maximum {{ max }} characters."
     * )
     * @Assert\Email(
     *     message = "VendorData e-mail '{{ value }}' is not a valid e-mail.",
     *     checkMX = true
     * )
     * @Assert\NotNull()
     */
    private $vendorEmail = '';

    /**
     * @var boolean
     *
     * @ORM\Column(name="vendor_send_proform", type="boolean", nullable=false)
     * @Assert\Choice(
     *     callback = "getSendProformValues",
     *     message = "This send proform value '{{ value }}' is not a valid send proform value."
     * )
     * @Assert\NotNull()
     */
    private $vendorSendProform = 0;

    /**
     * Set id
     *
     * @param int $id
     */
    public function setId($id)
    {
        $this->id = $id;
    }

    /**
     * Get id
     *
     * @return int
     */
    public function getId()
    {
        return $this->id;
    }

    /**
     * Set address
     *
     * @param string $vendorAddress
     */
    public function setVendorAddress($vendorAddress)
    {
        $this->vendorAddress = $vendorAddress;
    }

    /**
     * Get address
     *
     * @return string
     */
    public function getVendorAddress()
    {
        return $this->vendorAddress;
    }

    /**
     * Set bank
     *
     * @param string $vendorBank
     */
    public function setVendorBank($vendorBank)
    {
        $this->vendorBank = $vendorBank;
    }

    /**
     * Get bank
     *
     * @return string
     */
    public function getVendorBank()
    {
        return $this->vendorBank;
    }

    /**
     * Set capital
     *
     * @param float $vendorCapital
     */
    public function setVendorCapital($vendorCapital)
    {
        if (!isset($vendorCapital)) {
            $vendorCapital = 0.0;
        }
        $this->vendorCapital = $vendorCapital;
    }

    /**
     * Get capital
     *
     * @return float
     */
    public function getVendorCapital()
    {
        return $this->vendorCapital;
    }

    /**
     * Set cui
     *
     * @param string $vendorCui
     */
    public function setVendorCui($vendorCui)
    {
        $this->vendorCui = $vendorCui;
    }

    /**
     * Get cui
     *
     * @return string
     */
    public function getVendorCui()
    {
        return $this->vendorCui;
    }

    /**
     * Set e-mail
     *
     * @param string $vendorEmail
     */
    public function setVendorEmail($vendorEmail)
    {
        if (!isset($vendorEmail)) {
            $vendorEmail = '';
        }
        $this->vendorEmail = $vendorEmail;
    }

    /**
     * Get e-mail
     *
     * @return string
     */
    public function getVendorEmail()
    {
        return $this->vendorEmail;
    }

    /**
     * Set fax
     *
     * @param string $vendorFax
     */
    public function setVendorFax($vendorFax)
    {
        if (!isset($vendorFax)) {
            $vendorFax = '';
        }
        $this->vendorFax = $vendorFax;
    }

    /**
     * Get fax
     *
     * @return string
     */
    public function getVendorFax()
    {
        return $this->vendorFax;
    }

    /**
     * Set iban
     *
     * @param string $vendorIban
     */
    public function setVendorIban($vendorIban)
    {
        $this->vendorIban = $vendorIban;
    }

    /**
     * Get iban
     *
     * @return string
     */
    public function getVendorIban()
    {
        return $this->vendorIban;
    }

    /**
     * Set vendor id
     *
     * @param int $vendorId
     */
    public function setVendor($vendorId)
    {
        $this->vendor = $vendorId;
    }

    /**
     * Get vendor id
     *
     * @return int
     */
    public function getVendor()
    {
        return $this->vendor;
    }

    /**
     * Set name
     *
     * @param string $vendorName
     */
    public function setVendorName($vendorName)
    {
        $this->vendorName = $vendorName;
    }

    /**
     * Get name
     *
     * @return string
     */
    public function getVendorName()
    {
        return $this->vendorName;
    }

    /**
     * Set order number
     *
     * @param string $vendorOrderNumber
     */
    public function setVendorOrderNumber($vendorOrderNumber)
    {
        $this->vendorOrderNumber = $vendorOrderNumber;
    }

    /**
     * Get order number
     *
     * @return string
     */
    public function getVendorOrderNumber()
    {
        return $this->vendorOrderNumber;
    }

    /**
     * Set phone
     *
     * @param string $vendorPhone
     */
    public function setVendorPhone($vendorPhone)
    {
        if (!isset($vendorPhone)) {
            $vendorPhone = '';
        }
        $this->vendorPhone = $vendorPhone;
    }

    /**
     * Get phone
     *
     * @return string
     */
    public function getVendorPhone()
    {
        return $this->vendorPhone;
    }

    /**
     * Set send proform
     *
     * @param boolean $vendorSendProform
     */
    public function setVendorSendProform($vendorSendProform)
    {
        if (!isset($vendorSendProform)) {
            $vendorSendProform = 0;
        }
        $this->vendorSendProform = $vendorSendProform;
    }

    /**
     * Get send proform
     *
     * @return boolean
     */
    public function getVendorSendProform()
    {
        return $this->vendorSendProform;
    }

    /**
     * Get Vendor send profrom values
     *
     * @return array
     */
    public static function getSendProformValues() {
        return array(
            0 => 'Not send',
            1 => 'Send'
        );
    }

    /**
     * Get Valid Vendor send profrom values
     *
     * @return array
     */
    public static function getValidSendProformValues() {
        return array(
            0, // 'Not send'
            1 // 'Send'
        );
    }

}
